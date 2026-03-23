SUMMARY = "Junglami system control daemon and CLI"
DESCRIPTION = "radxactl exposes system management (power, wifi, bluetooth, audio, aic8800) \
via D-Bus as a daemon, and provides a CLI client for the same interface."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "systemd pkgconfig-native"

SRC_URI = " \
    file://CMakeLists.txt \
    file://radxactl.service \
    file://cr.junglami.radxactl.conf \
    file://src/main.c \
    file://src/daemon/bus.h \
    file://src/daemon/bus.c \
    file://src/daemon/modules/power.h \
    file://src/daemon/modules/power.c \
    file://src/daemon/modules/bluetooth.h \
    file://src/daemon/modules/bluetooth.c \
    file://src/daemon/modules/wifi.h \
    file://src/daemon/modules/wifi.c \
    file://src/daemon/modules/audio.h \
    file://src/daemon/modules/audio.c \
    file://src/daemon/modules/aic8800.h \
    file://src/daemon/modules/aic8800.c \
    file://src/cli/cli.h \
    file://src/cli/cli.c \
"

S = "${WORKDIR}"

inherit cmake systemd

SYSTEMD_SERVICE:${PN} = "radxactl.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${datadir}/dbus-1/system.d
    install -m 0644 ${WORKDIR}/cr.junglami.radxactl.conf \
        ${D}${datadir}/dbus-1/system.d/
}

FILES:${PN} += " \
    ${bindir}/radxactl \
    ${systemd_unitdir}/system/radxactl.service \
    ${datadir}/dbus-1/system.d/cr.junglami.radxactl.conf \
"
