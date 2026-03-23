FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://nsswitch.conf"

do_install:append() {
    install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/nsswitch.conf
}
