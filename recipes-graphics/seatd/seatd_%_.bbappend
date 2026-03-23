FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://seatd.service"

inherit systemd

SYSTEMD_SERVICE:${PN} = "seatd.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/seatd.service \
        ${D}${systemd_unitdir}/system/seatd.service
}

FILES:${PN} += "${systemd_unitdir}/system/seatd.service"
