#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "power.h"
#include "../bus.h"

static int call_logind(sd_bus_message *m, sd_bus_error *err, const char *method) {
    sd_bus_message *reply = NULL;
    int r = sd_bus_call_method(bus_get(),
            "org.freedesktop.login1",
            "/org/freedesktop/login1",
            "org.freedesktop.login1.Manager",
            method, err, &reply, "b", 0);
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: power/%s failed: %s\n",
                method, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "logind call failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_power_reboot(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: reboot\n");
    return call_logind(m, err, "Reboot");
}

int method_power_shutdown(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: shutdown\n");
    return call_logind(m, err, "PowerOff");
}

int method_power_suspend(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: suspend\n");
    return call_logind(m, err, "Suspend");
}
