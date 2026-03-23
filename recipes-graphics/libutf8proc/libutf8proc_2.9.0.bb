SUMMARY = "Unicode processing library"
HOMEPAGE = "https://github.com/JuliaStrings/utf8proc"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE.md;md5=96d5a3ba306e0f24fb289427af484408"

SRC_URI = "https://github.com/JuliaStrings/utf8proc/archive/refs/tags/v2.9.0.tar.gz;downloadfilename=utf8proc-2.9.0.tar.gz"
SRC_URI[sha256sum] = "18c1626e9fc5a2e192311e36b3010bfc698078f692888940f1fa150547abb0c1"

S = "${WORKDIR}/utf8proc-2.9.0"

inherit cmake pkgconfig