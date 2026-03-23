#pragma once
#include <systemd/sd-bus.h>

int method_aic8800_set_antenna(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_aic8800_reset(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_aic8800_status(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_aic8800_set_txpower(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_aic8800_set_region(sd_bus_message *m, void *userdata, sd_bus_error *err);