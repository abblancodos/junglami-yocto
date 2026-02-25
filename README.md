# junglami-yocto

A custom Yocto BSP layer for the Radxa Zero 3W (Rockchip RK3566), targeting embedded audio/DSP applications with WiFi, Bluetooth, and HDMI audio support on mainline Linux 6.12 LTS.

## Hardware

- **SBC**: Radxa Zero 3W (RK3566, quad-core Cortex-A55, 4GB LPDDR4)
- **WiFi/BT**: AIC8800DS2 (SDIO WiFi + UART Bluetooth)
- **Audio DSP**: ADAU1701 (planned)
- **Amplifier**: STA350BW Class-D 3.1ch (planned)
- **HDMI**: Native RK3566 HDMI with I2S audio

---

## Layer Structure

```
junglami-yocto/
├── meta-dsp-playground/
│   ├── conf/
│   │   └── layer.conf
│   ├── recipes-bsp/
│   │   └── u-boot/          # U-Boot bbappend for Radxa Zero 3W
│   ├── recipes-connectivity/
│   │   └── aic8800/         # Out-of-tree AIC8800 WiFi/BT driver
│   └── recipes-kernel/
│       └── linux/           # Kernel bbappend + defconfig
```

---

## What Was Built and Why

### 1. Custom Kernel (Linux 6.12 LTS)

**Why**: The Radxa vendor kernel is based on a heavily patched 5.10 fork. For long-term maintainability and upstream compatibility, we target Linux 6.12 LTS using the `linux-junglami-dsp-plgnd` recipe with a minimal defconfig tailored for the RK3566 platform.

**Key decisions**:
- `PREFERRED_PROVIDER_virtual/kernel` overrides the default Radxa kernel
- Custom defconfig at `recipes-kernel/linux/linux-junglami-dsp-plgnd/dsp_defconfig`
- Keeps `CONFIG_MODULES=y` for out-of-tree driver support

---

### 2. AIC8800 WiFi/BT Driver

**Why**: The AIC8800DS2 chip has no upstream kernel driver. The only available source is a vendor out-of-tree driver derived from Debian packaging. On mainline kernels, this driver requires significant patching to build and function correctly.

**Recipe**: `recipes-connectivity/aic8800/aic8800-driver_git.bb`

**Patches applied** (in order):

1. `0001-mainline-replace-rfkill-wlan-Android-header-with-inline-stub.patch` — Replaces Android-specific `rfkill-wlan.h` with a stub header implementing the Rockchip WiFi power/carddetect API using `mmc_detect_change()`.
2. `0002-fix-sdio-firmware-path.patch` through `0010-fix-linux-6.15-build.patch` — Debian packaging patches ported to git format-patch, fixing firmware paths, cfg80211 API changes, and build errors on kernels >= 6.12.

**Build flags**:
- `CONFIG_PLATFORM_ROCKCHIP2=y` — Enables the Rockchip power sequencing code path
- `CONFIG_SDIO_FIRMWARE_PATH` — Points to `/lib/firmware/aic8800_fw/SDIO/aic8800D80/`

---

### 3. WiFi Initialization Fix

**Problem**: The AIC8800 SDIO driver is shared between Bluetooth and WiFi subsystems. On boot, the BSP module (`aic8800_bsp`) initializes for Bluetooth, then hands off to the WiFi driver (`aic8800_fdrv`). On mainline kernels, the SDIO card is declared `non-removable` in the DTS, which prevents `mmc_detect_change()` from triggering a re-probe. This caused WiFi to always fail with a 2-second timeout.

**Root cause**: Three compounding issues:

1. `rockchip_wifi_set_carddetect()` was called before the SDIO notify semaphore was registered, causing the probe signal to be missed.
2. `aicbsp_sdio_exit()` was conditionally skipped for Rockchip platforms (`#ifndef CONFIG_PLATFORM_ROCKCHIP`), but not for `CONFIG_PLATFORM_ROCKCHIP2`, leaving the SDIO driver registered between BT and WiFi sessions.
3. `aicbsp_platform_power_on()` always waited on `aic_chipup_sem` for a new probe event, but since the driver was already bound and the card non-removable, no probe would fire on the second call.

**Fix** (in `aicsdio.c`):

- Extended the `#ifndef CONFIG_PLATFORM_ROCKCHIP` guard to also exclude `CONFIG_PLATFORM_ROCKCHIP2`, so `aicbsp_sdio_exit()` is skipped and the driver stays registered across BT/WiFi transitions.
- Added early return in `aicbsp_platform_power_on()`: if `aicbsp_sdiodev != NULL`, the device is already probed — skip the semaphore wait entirely.
- Added early return in `aicbsp_sdio_init()`: if `aicbsp_sdiodev != NULL`, skip `sdio_register_driver()` — the driver is already bound.

These three changes together allow WiFi to reuse the already-probed SDIO device without needing a re-enumeration that the `non-removable` DTS property would block.

---

### 4. Bluetooth (UART)

**Why UART and not SDIO BT**: The AIC8800DS2 exposes Bluetooth over UART (HCI), not over the SDIO interface directly. The `aic8800_btlpm` module handles power management, but the HCI transport requires `hciattach` on the correct UART port.

**UART mapping on RK3566**:
- `fe650000.serial` → `/dev/ttyS1` — Bluetooth HCI (correct port)
- `fe660000.serial` → `/dev/ttyS2` — Debug console (do NOT use for BT)

**Bring-up sequence**:
```bash
hciattach -s 1500000 /dev/ttyS1 any 1500000 flow nosleep &
rfkill unblock bluetooth
hciconfig hci0 up
systemctl start bluetooth
```

---

### 5. PipeWire + WirePlumber + A2DP Sink

**Why PipeWire**: PipeWire provides a unified audio graph that can simultaneously handle ALSA (HDMI), Bluetooth A2DP, and routing between them without manual configuration.

**Packages required**:
```
pipewire
pipewire-alsa
pipewire-pulse
pipewire-spa-plugins-alsa
pipewire-spa-plugins-bluez5
pipewire-spa-plugins-codec-bluez5-sbc
pipewire-spa-plugins-codec-bluez5-aac
pipewire-spa-plugins-audioconvert
pipewire-spa-plugins-audiomixer
pipewire-spa-plugins-support
pipewire-modules-protocol-native
pipewire-modules-protocol-pulse
pipewire-modules-client-node
pipewire-modules-client-device
pipewire-modules-adapter
pipewire-modules-metadata
pipewire-modules-session-manager
wireplumber
```

**Headless session workaround**: PipeWire and WirePlumber are user-space services that require a D-Bus session bus and a valid `XDG_RUNTIME_DIR`. Running as root without a login session causes WirePlumber's BlueZ monitor to stop itself because logind reports no active seat.

Solution — create a dedicated user and set up the runtime directory:
```bash
useradd -m -s /bin/sh -G audio,video junglami
mkdir -p /run/user/1000
chown junglami:junglami /run/user/1000
chmod 700 /run/user/1000
```

Disable seat monitoring in WirePlumber:
```
# /etc/wireplumber/wireplumber.conf.d/10-no-logind.conf
wireplumber.profiles = {
  main = {
    monitor.bluez.seat-monitoring = disabled
    monitor.bluez-midi.seat-monitoring = disabled
  }
}
```

Run as the junglami user:
```bash
export XDG_RUNTIME_DIR=/run/user/1000
dbus-run-session -- sh
pipewire &
sleep 1
wireplumber &
```

Once running, WirePlumber automatically registers A2DP Sink endpoints (`/MediaEndpoint/A2DPSink/sbc`, `/MediaEndpoint/A2DPSink/aac`) with BlueZ. Phones can then connect and stream audio, which WirePlumber routes to the HDMI sink.

---

## Audio Chain

```
Phone (BT source)
    → AIC8800DS2 (A2DP over BR/EDR)
        → aic8800_btlpm + hciattach /dev/ttyS1
            → BlueZ HCI
                → PipeWire spa-bluez5 (A2DP Sink)
                    → WirePlumber (routing)
                        → PipeWire ALSA sink
                            → RK3566 I2S (fe400000)
                                → HDMI audio out
```

---

## Known Issues and Workarounds

| Issue | Cause | Workaround |
|-------|-------|------------|
| WiFi fails after BT teardown | non-removable SDIO, stale driver binding | Skip power-on semaphore wait if sdiodev already present |
| BT not up after reboot | hciattach not persistent | Run manually or add systemd service |
| PipeWire won't start as root | No D-Bus session bus, no XDG_RUNTIME_DIR | Use dedicated `junglami` user |
| WirePlumber BlueZ monitor stops | logind reports no active seat | Disable seat monitoring in wireplumber.conf.d |
| Regulatory domain warning | Driver uses permissive custom rules | Set country code in `aic_userconfig_8800d80.txt` |

---

## Building

```bash
# Clone
git clone https://github.com/youruser/junglami-yocto
cd poky-radxa/poky
source oe-init-build-env build

# Add layer
bitbake-layers add-layer ../../junglami-yocto/meta-dsp-playground

# Build
bitbake core-image-base
```

### local.conf essentials

```bitbake
MACHINE = "radxa-zero-3w"
DISTRO = "poky"
PREFERRED_PROVIDER_virtual/kernel:forcevariable = "linux-junglami-dsp-plgnd"
PREFERRED_VERSION_linux-junglami-dsp-plgnd:forcevariable = "6.12%"
DISTRO_FEATURES:append = " systemd usrmerge wifi bluetooth"
VIRTUAL-RUNTIME_init_manager = "systemd"
LICENSE_FLAGS_ACCEPTED += "commercial"

IMAGE_INSTALL:append = " \
    pipewire pipewire-alsa pipewire-pulse \
    pipewire-spa-plugins-alsa pipewire-spa-plugins-bluez5 \
    pipewire-spa-plugins-codec-bluez5-sbc pipewire-spa-plugins-codec-bluez5-aac \
    pipewire-spa-plugins-audioconvert pipewire-spa-plugins-audiomixer \
    pipewire-spa-plugins-support \
    pipewire-modules-protocol-native pipewire-modules-protocol-pulse \
    pipewire-modules-client-node pipewire-modules-client-device \
    pipewire-modules-adapter pipewire-modules-metadata \
    pipewire-modules-session-manager \
    wireplumber \
    alsa-utils bluez5 bluez5-obex \
    aic8800-driver \
    openssh openssh-sftp-server \
    iw wpa-supplicant linux-firmware \
"
```

---

## References

- [Radxa Zero 3W hardware docs](https://docs.radxa.com/en/zero/zero3)
- [AIC8800 Debian packaging](https://salsa.debian.org/kernel-team/aic8800)
- [radxa-pkg/aic8800](https://github.com/radxa-pkg/aic8800)
- [PipeWire / WirePlumber documentation](https://pipewire.pages.freedesktop.org/wireplumber/)
- Linux 6.12 LTS — kernel.org
