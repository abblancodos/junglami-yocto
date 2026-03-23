do_configure() {
    mkdir -p ${B}/meson-logs
    touch ${B}/meson-logs/meson-log.txt
}

do_compile() {
    mkdir -p ${B}
    echo "void __libglu_stub(void) {}" > ${B}/glu_stub.c
    ${CC} ${CFLAGS} -shared -fPIC -Wl,-soname,libGLU.so.1 \
        -o ${B}/libGLU.so.1.3.1 ${B}/glu_stub.c
    ln -sf libGLU.so.1.3.1 ${B}/libGLU.so.1
    ln -sf libGLU.so.1     ${B}/libGLU.so
}

do_install() {
    install -d ${D}${libdir}
    install -d ${D}${libdir}/pkgconfig
    install -d ${D}${includedir}/GL

    install -m 0755 ${B}/libGLU.so.1.3.1 ${D}${libdir}/
    ln -sf libGLU.so.1.3.1 ${D}${libdir}/libGLU.so.1
    ln -sf libGLU.so.1     ${D}${libdir}/libGLU.so

    cat > ${D}${libdir}/pkgconfig/glu.pc << PCEOF
prefix=/usr
exec_prefix=\${prefix}
libdir=\${prefix}/lib
includedir=\${prefix}/include

Name: glu
Description: GLU stub for GLES-only builds
Version: 9.0.3
Libs: -L\${libdir} -lGLU
Cflags: -I\${includedir}
PCEOF
}
