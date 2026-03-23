SUMMARY = "Seat management library and daemon"
HOMEPAGE = "https://git.sr.ht/~kennylevinsen/seatd"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=80c946af1f5eddde0eb8a9e35e1b3b04"

SRC_URI = "https://git.sr.ht/~kennylevinsen/seatd/archive/0.8.0.tar.gz"
SRC_URI[sha256sum] = "c5f0f02e02639563fa55866ede33e8dd6c9d6cddc0b49d38e6fb85e596fad3d9"

S = "${WORKDIR}/seatd-0.8.0"

inherit meson pkgconfig systemd

EXTRA_OEMESON = " \
    -Dlibseat-logind=systemd \
    -Dlibseat-builtin=enabled \
    -Dserver=enabled \
    -Dexamples=disabled \
    -Dman-pages=disabled \
"

DEPENDS = "systemd"

SYSTEMD_SERVICE:${PN} = "seatd.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

PACKAGES =+ "${PN}-server ${PN}-dev"

FILES:${PN} = "${libdir}/libseat.so.*"
FILES:${PN}-server = " \
    ${sbindir}/seatd \
    ${systemd_system_unitdir}/seatd.service \
"
FILES:${PN}-dev = " \
    ${includedir}/libseat.h \
    ${libdir}/libseat.so \
    ${libdir}/pkgconfig/libseat.pc \
"