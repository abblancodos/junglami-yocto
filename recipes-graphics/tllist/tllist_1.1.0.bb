SUMMARY = "Type-checked linked list in C"
HOMEPAGE = "https://codeberg.org/dnkl/tllist"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3a7351a597a91e763901f7c76f21e798"

SRC_URI = "https://codeberg.org/dnkl/tllist/archive/1.1.0.tar.gz;downloadfilename=tllist-1.1.0.tar.gz"
SRC_URI[sha256sum] = "0e7b7094a02550dd80b7243bcffc3671550b0f1d8ba625e4dff52517827d5d23"

S = "${WORKDIR}/tllist"

inherit meson pkgconfig

do_install:append() {
    install -d ${D}${includedir}
    install -m 0644 ${S}/tllist.h ${D}${includedir}/tllist.h
}

FILES:${PN}-dev += "${includedir}/tllist.h"