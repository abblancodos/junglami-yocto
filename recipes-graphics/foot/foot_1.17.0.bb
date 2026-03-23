SUMMARY = "Fast Wayland terminal emulator"
HOMEPAGE = "https://codeberg.org/dnkl/foot"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3a7351a597a91e763901f7c76f21e798"

DEPENDS = " \
    fcft \
    fontconfig \
    libutf8proc \
    pixman \
    scdoc-native \
    tllist \
    wayland \
    wayland-native \
    wayland-protocols \
    libxkbcommon \
"

SRC_URI = "https://codeberg.org/dnkl/foot/archive/1.17.0.tar.gz;downloadfilename=foot-1.17.0.tar.gz"
SRC_URI[sha256sum] = "ab5603edad89d26e8ae543e92c72210a1fbbe7abdf15a0729664b41baed6a3b8"

S = "${WORKDIR}/foot"

inherit meson pkgconfig

EXTRA_OEMESON = " \
    -Dime=false \
    -Dthemes=false \
    -Dterminfo=disabled \
"

FILES:${PN} += " \
    ${bindir}/foot \
    ${bindir}/footclient \
    ${datadir}/foot \
    ${datadir}/icons \
    ${datadir}/bash-completion \
    ${datadir}/zsh \
    ${datadir}/fish \
"