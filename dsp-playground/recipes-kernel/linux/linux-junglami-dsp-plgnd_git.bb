SUMMARY = "Linux 6.12 LTS for Radxa Zero 3W"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=6bc538ed5bd9a7fc9398086aedcd7e46"

inherit kernel

LINUX_VERSION = "6.12.74"
LINUX_VERSION_EXTENSION = "-dsp"
PV = "${LINUX_VERSION}+git${SRCPV}"

SRCREV = "${AUTOREV}"

SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git;branch=linux-6.12.y;protocol=https \
    file://dsp_defconfig \
"

S = "${WORKDIR}/git"

COMPATIBLE_MACHINE = "radxa-zero-3w"

KCONFIG_MODE = "alldefconfig"
KBUILD_DEFCONFIG = "dsp_defconfig"

# meta-rockchip wants a FIT image — keep that expectation
KERNEL_CLASSES = "kernel-fitimage"
KERNEL_IMAGETYPE = "fitImage"

# The rockchip SCC that strips non-vendor DTS won't exist in mainline source
KERNEL_FEATURES:remove = "bsp/rockchip/remove-non-rockchip-arch-arm64.scc"