SUMMARY = "Junglami OS session setup"
DESCRIPTION = "Creates the junglami user, configures greetd with agreety, \
sets up PipeWire/WirePlumber as junglami user, and handles BT UART bring-up."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit junglami-user systemd

RDEPENDS:${PN} += "greetd bash labwc foot seatd"

SRC_URI = " \
    file://bluetooth-init.service \
    file://junglami-pipewire.service \
    file://junglami-pipewire.conf \
    file://junglami-tmpfiles.conf \
    file://labwc-autostart \
    file://85-junglami.preset \
"

S = "${WORKDIR}"

SYSTEMD_SERVICE:${PN} = " \
    bluetooth-init.service \
    junglami-pipewire.service \
"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
    # systemd units
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/bluetooth-init.service \
        ${D}${systemd_unitdir}/system/
    install -m 0644 ${WORKDIR}/junglami-pipewire.service \
        ${D}${systemd_unitdir}/system/

    # systemd preset
    install -d ${D}/usr/lib/systemd/system-preset
    install -m 0644 ${WORKDIR}/85-junglami.preset \
    ${D}/usr/lib/systemd/system-preset/85-junglami.preset

    # WirePlumber: deshabilitar seat monitoring
    install -d ${D}${sysconfdir}/wireplumber/wireplumber.conf.d
    install -m 0644 ${WORKDIR}/junglami-pipewire.conf \
        ${D}${sysconfdir}/wireplumber/wireplumber.conf.d/10-no-logind.conf

    # labwc autostart
    install -d ${D}${sysconfdir}/xdg/labwc
    install -m 0755 ${WORKDIR}/labwc-autostart \
        ${D}${sysconfdir}/xdg/labwc/autostart

    # tmpfiles para XDG_RUNTIME_DIR
    install -d ${D}${libdir}/tmpfiles.d
    install -m 0644 ${WORKDIR}/junglami-tmpfiles.conf \
        ${D}${libdir}/tmpfiles.d/junglami.conf
}

FILES:${PN} += " \
    ${systemd_unitdir}/system/bluetooth-init.service \
    ${systemd_unitdir}/system/junglami-pipewire.service \
    /usr/lib/systemd/system-preset/85-junglami.preset \
    ${sysconfdir}/wireplumber/wireplumber.conf.d/10-no-logind.conf \
    ${sysconfdir}/xdg/labwc/autostart \
    ${libdir}/tmpfiles.d/junglami.conf \
"
