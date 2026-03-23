SUMMARY = "Font loading and glyph rasterization library"
HOMEPAGE = "https://codeberg.org/dnkl/fcft"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=3a7351a597a91e763901f7c76f21e798"

DEPENDS = " \
    fontconfig \
    freetype \
    harfbuzz \
    libutf8proc \
    pixman \
    tllist \
    scdoc-native \
"

SRC_URI = "https://codeberg.org/dnkl/fcft/archive/3.1.9.tar.gz;downloadfilename=fcft-3.1.9.tar.gz"
SRC_URI[sha256sum] = "4b7e3b2ab7e14f532d8a9cb0f2d3b0cdf9d2919b95e6ab8030f7ac87d059c2b6"

S = "${WORKDIR}/fcft"

inherit meson pkgconfig

EXTRA_OEMESON = " \
    -Dsvg-backend=none \
    -Dtest-text-shaping=false \
"

FILES:${PN} += "${libdir}/libfcft*.so.*"
FILES:${PN}-dev += "${libdir}/libfcft*.so"