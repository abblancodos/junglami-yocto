PASSWD_JUNGLAMI = "\$5\$ISEESk1CEcoF6UfF\$0m4B.GJiye.gxk172FDOtncYFGQ2GpJpzEiblbWZcvB"
PASSWD_ROOT = "\$5\$WabHKZj15gWhz02C\$ma.ZtIESJfxbAVgi42JGejrmb0Ewjzl0swHLHuFXAs9"

EXTRA_USERS_PARAMS = "\
    usermod -p '${PASSWD_JUNGLAMI}' junglami; \
    usermod -p '${PASSWD_ROOT}' root; \
    "