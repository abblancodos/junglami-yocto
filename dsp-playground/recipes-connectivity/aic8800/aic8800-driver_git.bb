SUMMARY = "AIC8800DS2 SDIO WiFi/BT kernel modules and firmware"
LICENSE = "GPL-3.0-only"
LIC_FILES_CHKSUM = "file://LICENSE;md5=1ebbd3e34237af26da5dc08a4e440464"

inherit module

SRC_URI = "git://github.com/radxa-pkg/aic8800.git;branch=main;protocol=https \
           file://fix-sdio-firmware-path.patch \
           file://fix-sdio-fall-through.patch \
           file://fix-linux-6.1-build.patch \
           file://fix-linux-6.5-build.patch \
           file://fix-linux-6.7-build.patch \
           file://fix-linux-6.9-build.patch \
           file://fix-linux-6.12-build.patch \
           file://fix-linux-6.13-build.patch \
           file://fix-linux-6.14-build.patch \
           file://fix-linux-6.15-build.patch \
           file://0001-mainline-replace-rfkill-wlan-Android-header-with-inl.patch \
           "

SRCREV = "7af8ef10bd0222a4e1ee54c25904d5e0e3ca37f7"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += " \
    ARCH=arm64 \
    CROSS_COMPILE=${TARGET_PREFIX} \
    KDIR=${STAGING_KERNEL_DIR} \
    CONFIG_PLATFORM_ROCKCHIP=y \
    CONFIG_PLATFORM_UBUNTU=n \
    CONFIG_PLATFORM_ROCKCHIP2=y \
"

do_compile() {
    cd ${S}/src/SDIO/driver_fw/driver/aic8800
    oe_runmake ARCH=arm64 \
        CROSS_COMPILE=${TARGET_PREFIX} \
        KDIR=${STAGING_KERNEL_DIR} \
        CONFIG_PLATFORM_ROCKCHIP=y \
        CONFIG_PLATFORM_UBUNTU=n \
    CONFIG_PLATFORM_ROCKCHIP2=y
}

do_install() {
    install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/src/SDIO/driver_fw/driver/aic8800/aic8800_bsp/aic8800_bsp.ko \
        ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/src/SDIO/driver_fw/driver/aic8800/aic8800_fdrv/aic8800_fdrv.ko \
        ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
    install -m 0644 ${S}/src/SDIO/driver_fw/driver/aic8800/aic8800_btlpm/aic8800_btlpm.ko \
        ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/

    install -d ${D}${nonarch_base_libdir}/firmware/aic8800_fw/SDIO/aic8800D80/
    install -m 0644 ${S}/src/SDIO/driver_fw/fw/aic8800D80/* \
        ${D}${nonarch_base_libdir}/firmware/aic8800_fw/SDIO/aic8800D80/
}

FILES:${PN} += " \
    ${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/ \
    ${nonarch_base_libdir}/firmware/aic8800_fw/ \
"

KERNEL_MODULE_AUTOLOAD += "aic8800_bsp aic8800_btlpm aic8800_fdrv"

INSANE_SKIP:${PN} += "buildpaths"
INSANE_SKIP:${PN}-dbg += "buildpaths"