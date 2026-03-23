SUMMARY = "Junglami OS graphics package group"
DESCRIPTION = "Mesa, DRM and graphics stack required by both WPE and RetroArch."
LICENSE = "MIT"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

RDEPENDS:${PN} = " \
    libdrm \
    mesa \
    mesa-megadriver \
"
