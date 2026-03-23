SUMMARY = "KMS plane offloading library"
HOMEPAGE = "https://gitlab.freedesktop.org/emersion/libliftoff"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=706cd9899438a9385250ab6773c1fa53"

DEPENDS = "libdrm"

SRC_URI = "https://gitlab.freedesktop.org/emersion/libliftoff/-/archive/v0.4.1/libliftoff-v0.4.1.tar.gz"
SRC_URI[sha256sum] = "44ec5cfdd0df040d1023d4d6a48b23c31f21ce61ee2347da9e1ca244fe24dd1c"

S = "${WORKDIR}/libliftoff-v0.4.1"

inherit meson pkgconfig