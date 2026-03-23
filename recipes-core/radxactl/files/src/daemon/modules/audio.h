#pragma once
#include <systemd/sd-bus.h>

int method_audio_set_volume(sd_bus_message *m, void *userdata, sd_bus_error *err);
int method_audio_status(sd_bus_message *m, void *userdata, sd_bus_error *err);
