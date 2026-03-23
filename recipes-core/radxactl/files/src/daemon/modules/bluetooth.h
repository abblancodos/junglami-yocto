#pragma once
#include <systemd/sd-bus.h>

int method_bt_enable(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_disable(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_set_sink_mode(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_scan(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_status(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_pair(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_unpair(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_connect(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_disconnect(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_set_discoverable(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_bt_set_antenna(sd_bus_message *m, void *userdata, sd_bus_error *err);