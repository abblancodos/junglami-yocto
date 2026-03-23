#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "bus.h"
#include "modules/power.h"
#include "modules/bluetooth.h"
#include "modules/wifi.h"
#include "modules/audio.h"
#include "modules/aic8800.h"

static sd_bus *_bus = NULL;

sd_bus *bus_get(void) { return _bus; }

int bus_emit_signal(const char *signal, const char *type, ...) {
    sd_bus_message *msg = NULL;
    int r = sd_bus_message_new_signal(_bus, &msg,
                RADXACTL_OBJECT_PATH, RADXACTL_INTERFACE, signal);
    if (r < 0) {
        fprintf(stderr, "radxactl: signal %s failed: %s\n", signal, strerror(-r));
        return r;
    }
    r = sd_bus_send(_bus, msg, NULL);
    sd_bus_message_unref(msg);
    return r;
}

static const sd_bus_vtable radxactl_vtable[] = {
    SD_BUS_VTABLE_START(0),

    /* Power */
    SD_BUS_METHOD("Reboot",   "", "bs", method_power_reboot,   SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("Shutdown", "", "bs", method_power_shutdown, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("Suspend",  "", "bs", method_power_suspend,  SD_BUS_VTABLE_UNPRIVILEGED),

    /* Bluetooth */
    SD_BUS_METHOD("BluetoothEnable",          "",  "bs", method_bt_enable,           SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothDisable",         "",  "bs", method_bt_disable,          SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothScan",            "",  "bs", method_bt_scan,             SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothStatus",          "",  "bs", method_bt_status,           SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothPair",            "s", "bs", method_bt_pair,             SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothUnpair",          "s", "bs", method_bt_unpair,           SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothConnect",         "s", "bs", method_bt_connect,          SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothDisconnect",      "s", "bs", method_bt_disconnect,       SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothSetDiscoverable", "b", "bs", method_bt_set_discoverable, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("BluetoothSetSinkMode",     "b", "bs", method_bt_set_sink_mode,    SD_BUS_VTABLE_UNPRIVILEGED),

    /* WiFi */
    SD_BUS_METHOD("WifiScan",    "",   "bs", method_wifi_scan,    SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("WifiConnect", "ss", "bs", method_wifi_connect, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("WifiForget",  "s",  "bs", method_wifi_forget,  SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("WifiStatus",  "",   "bs", method_wifi_status,  SD_BUS_VTABLE_UNPRIVILEGED),

    /* Audio */
    SD_BUS_METHOD("AudioSetVolume", "u", "bs", method_audio_set_volume, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("AudioStatus",    "",  "bs", method_audio_status,     SD_BUS_VTABLE_UNPRIVILEGED),

    /* AIC8800 */
    SD_BUS_METHOD("AIC8800SetAntenna", "u", "bs", method_aic8800_set_antenna, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("AIC8800Reset",      "",  "bs", method_aic8800_reset,       SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("AIC8800Status",     "",  "bs", method_aic8800_status,      SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("AIC8800SetTxPower", "u", "bs", method_aic8800_set_txpower, SD_BUS_VTABLE_UNPRIVILEGED),
    SD_BUS_METHOD("AIC8800SetRegion",  "s", "bs", method_aic8800_set_region,  SD_BUS_VTABLE_UNPRIVILEGED),

    /* Señales async */
    SD_BUS_SIGNAL("WifiStateChanged",      "s", 0),
    SD_BUS_SIGNAL("BluetoothStateChanged", "s", 0),
    SD_BUS_SIGNAL("AudioVolumeChanged",    "u", 0),

    SD_BUS_VTABLE_END
};

int daemon_run(void) {
    int r;

    r = sd_bus_open_system(&_bus);
    if (r < 0) {
        fprintf(stderr, "radxactl: cannot connect to system bus: %s\n", strerror(-r));
        return r;
    }

    r = sd_bus_add_object_vtable(_bus, NULL,
            RADXACTL_OBJECT_PATH, RADXACTL_INTERFACE,
            radxactl_vtable, NULL);
    if (r < 0) {
        fprintf(stderr, "radxactl: add vtable failed: %s\n", strerror(-r));
        goto finish;
    }

    r = sd_bus_request_name(_bus, RADXACTL_BUS_NAME, 0);
    if (r < 0) {
        fprintf(stderr, "radxactl: cannot acquire '%s': %s\n",
                RADXACTL_BUS_NAME, strerror(-r));
        goto finish;
    }

    fprintf(stdout, "radxactl: daemon running on %s\n", RADXACTL_BUS_NAME);

    for (;;) {
        r = sd_bus_process(_bus, NULL);
        if (r < 0) {
            fprintf(stderr, "radxactl: process error: %s\n", strerror(-r));
            goto finish;
        }
        if (r > 0) continue;
        r = sd_bus_wait(_bus, UINT64_MAX);
        if (r < 0) {
            fprintf(stderr, "radxactl: wait error: %s\n", strerror(-r));
            goto finish;
        }
    }

finish:
    sd_bus_unref(_bus);
    return r < 0 ? 1 : 0;
}