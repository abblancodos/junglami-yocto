SUMMARY = "Junglami OS audio package group"
DESCRIPTION = "PipeWire, WirePlumber and Bluetooth audio support for Junglami OS."
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    alsa-utils \
    pipewire \
    pipewire-alsa \
    pipewire-modules-adapter \
    pipewire-modules-client-device \
    pipewire-modules-client-node \
    pipewire-modules-metadata \
    pipewire-modules-protocol-native \
    pipewire-modules-protocol-pulse \
    pipewire-modules-session-manager \
    pipewire-pulse \
    pipewire-spa-plugins-alsa \
    pipewire-spa-plugins-audioconvert \
    pipewire-spa-plugins-audiomixer \
    pipewire-spa-plugins-bluez5 \
    pipewire-spa-plugins-codec-bluez5-aac \
    pipewire-spa-plugins-codec-bluez5-sbc \
    pipewire-spa-plugins-support \
    wireplumber \
"
