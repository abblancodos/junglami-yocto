# meta-dsp-playground/recipes-bsp/u-boot/u-boot_%.bbappend
DEPENDS:append = " gnutls-native"

SRCREV_Kwiboo = "af53363ba761abd110f5e80c8a7fcb1170e6f35e"

SRC_URI:remove = "git://github.com/Kwiboo/u-boot-rockchip.git;protocol=https;branch=rk3xxx-2024.07;name=Kwiboo"
SRC_URI:prepend = "git://github.com/Kwiboo/u-boot-rockchip.git;protocol=https;branch=rk3xxx-2024.10;name=Kwiboo "