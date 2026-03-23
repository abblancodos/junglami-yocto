#pragma once
#include <systemd/sd-bus.h>

int method_power_reboot(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_power_shutdown(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_power_suspend(sd_bus_message *m, void *userdata, sd_bus_error *err);