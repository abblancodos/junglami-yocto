SUMMARY = "Junglami OS connectivity package group"
DESCRIPTION = "WiFi, Bluetooth and network tools for Junglami OS."
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    aic8800-driver \
    bluez5 \
    bluez5-obex \
    iw \
    linux-firmware \
    wpa-supplicant \
"
