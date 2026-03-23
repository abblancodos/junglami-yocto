SUMMARY = "A modular Wayland compositor library"
HOMEPAGE = "https://gitlab.freedesktop.org/wlroots/wlroots"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=89e064f90bcb87796ca335cbd2ce4179"

DEPENDS = " \
    hwdata \
    libdisplay-info \
    libdrm \
    libinput \
    libliftoff \
    libxkbcommon \
    mesa \
    pixman \
    seatd \
    wayland \
    wayland-native \
    wayland-protocols \
"

SRC_URI = "https://gitlab.freedesktop.org/wlroots/wlroots/-/archive/0.17.4/wlroots-0.17.4.tar.bz2"
SRC_URI[sha256sum] = "76cea5143c966640000efad32d5961b1d2303744497d30ef5e78aaf50360288d"

S = "${WORKDIR}/wlroots-0.17.4"
PV = "0.17.4"

inherit meson pkgconfig

do_configure:prepend() {
    install -d ${RECIPE_SYSROOT_NATIVE}/usr/share/pkgconfig
    cp ${RECIPE_SYSROOT}/usr/share/pkgconfig/hwdata.pc \
       ${RECIPE_SYSROOT_NATIVE}/usr/share/pkgconfig/hwdata.pc
}

EXTRA_OEMESON = " \
    -Dxwayland=disabled \
    -Dbackends=drm,libinput \
    -Drenderers=gles2 \
    -Dallocators=gbm \
    -Dsession=enabled \
"

FILES:${PN} += "${libdir}/libwlroots*.so.*"
FILES:${PN}-dev += "${libdir}/libwlroots*.so"