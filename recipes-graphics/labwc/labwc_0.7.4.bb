SUMMARY = "A wlroots-based Wayland window-stacking compositor"
HOMEPAGE = "https://github.com/labwc/labwc"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=b234ee4d69f5fce4486a80fdaf4a4263"

DEPENDS = " \
    wlroots \
    wayland \
    wayland-native \
    wayland-protocols \
    libxkbcommon \
    libinput \
    cairo \
    pango \
    glib-2.0 \
    libxml2 \
    libpng \
    pixman \
"

SRC_URI = "https://github.com/labwc/labwc/archive/refs/tags/0.7.4.tar.gz;downloadfilename=labwc-0.7.4.tar.gz \
           file://labwc-autostart \
           file://labwc-rc.xml \
"
SRC_URI[sha256sum] = "2afa1ef483863fc6217803a540b9afab6939d2324e9bb4dcf4a028281b731fb9"

S = "${WORKDIR}/labwc-0.7.4"
PV = "0.7.4"

inherit meson pkgconfig

EXTRA_OEMESON = "-Dxwayland=disabled"

do_install:append() {
    install -d ${D}${datadir}/labwc
    install -m 0644 ${WORKDIR}/labwc-autostart ${D}${datadir}/labwc/autostart
    install -m 0644 ${WORKDIR}/labwc-rc.xml ${D}${datadir}/labwc/rc.xml
}

FILES:${PN} += " \
    ${datadir}/labwc \
    ${datadir}/wayland-sessions \
    ${datadir}/wayland-sessions/labwc.desktop \
    ${datadir}/icons \
"