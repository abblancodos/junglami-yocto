#pragma once
#include <systemd/sd-bus.h>

int method_wifi_scan(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_wifi_connect(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_wifi_forget(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_wifi_status(sd_bus_message *m, void *userdata, sd_bus_error *err);