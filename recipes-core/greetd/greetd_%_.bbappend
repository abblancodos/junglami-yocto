FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://config.toml file://greetd-pam"

inherit systemd

SYSTEMD_AUTO_ENABLE:${PN} = "enable"

# Remover el pam.d/greetd que instala greetd para poder pisarlo
CONFFILES:${PN}:remove = "${sysconfdir}/pam.d/greetd"

do_install:append() {
    # Config de greetd
    install -d ${D}${sysconfdir}/greetd
    install -m 0644 ${WORKDIR}/config.toml \
        ${D}${sysconfdir}/greetd/config.toml

    # PAM minimalista — solo pam_unix que está disponible en la imagen
    install -d ${D}${sysconfdir}/pam.d
    install -m 0644 ${WORKDIR}/greetd-pam \
        ${D}${sysconfdir}/pam.d/greetd
}

FILES:${PN} += " \
    ${sysconfdir}/greetd/config.toml \
    ${sysconfdir}/pam.d/greetd \
"
