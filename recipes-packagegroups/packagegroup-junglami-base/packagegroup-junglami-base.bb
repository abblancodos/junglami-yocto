SUMMARY = "Junglami OS base package group"
DESCRIPTION = "Core session, system daemon and tools. \
This is the minimum required to boot into a Junglami OS session."
LICENSE = "MIT"

inherit packagegroup

RDEPENDS:${PN} = " \
    junglami-session \
    openssh \
    openssh-sftp-server \
    radxactl \
    xkeyboard-config \
"
