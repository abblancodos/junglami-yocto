#pragma once
#include <systemd/sd-bus.h>

#define RADXACTL_BUS_NAME    "cr.junglami.radxactl"
#define RADXACTL_OBJECT_PATH "/cr/junglami/radxactl"
#define RADXACTL_INTERFACE   "cr.junglami.radxactl"

int daemon_run(void);
sd_bus *bus_get(void);
int bus_emit_signal(const char *signal, const char *type, ...);