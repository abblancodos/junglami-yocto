inherit useradd extrausers

JUNGLAMI_USER_ID      ?= "4242"
JUNGLAMI_USER_NAME    ??= "junglami"
JUNGLAMI_USER_HOMEDIR ?= "/home/${JUNGLAMI_USER_NAME}"
JUNGLAMI_USER_GROUPS  ?= "audio bluetooth dialout disk i2c input netdev plugdev seat tty users video"

RDEPENDS:${PN}:prepend = "bash "

GROUPADD_PARAM:${PN} = "i2c; netdev; bluetooth; -r seat"

JUNGLAMI_USERADD_COMMAND ?= " \
    --create-home \
    --user-group \
    --groups ${@','.join('${JUNGLAMI_USER_GROUPS}'.split())} \
    --uid ${JUNGLAMI_USER_ID} \
    --home ${JUNGLAMI_USER_HOMEDIR} \
    --shell /bin/bash \
    ${JUNGLAMI_USER_NAME} \
"

USERADD_PACKAGES = "${PN}"
USERADD_PARAM:${PN} = "${@' '.join('${JUNGLAMI_USERADD_COMMAND}'.split())}"

USERADD_PARAM:${PN} += "; \
    --system \
    --no-create-home \
    --home /dev/null \
    --shell /sbin/nologin \
    --user-group \
    greeter \
"