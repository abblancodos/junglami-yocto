#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "audio.h"
#include "../bus.h"

/*
 * TODO: implementar control de WirePlumber via D-Bus
 *
 * AudioSetVolume: llama WirePlumber pw-cli set-volume o via
 *                 org.freedesktop.pulseaudio (pipewire-pulse compat)
 * AudioStatus:    lee sink activo y volumen actual
 *
 * Emitir AudioVolumeChanged cuando el volumen cambie
 */

int method_audio_set_volume(sd_bus_message *m, void *u, sd_bus_error *e) {
    unsigned int vol = 0;
    int r = sd_bus_message_read(m, "u", &vol);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    if (vol > 100)
        return sd_bus_reply_method_return(m, "bs", 0, "volume must be 0-100");
    fprintf(stdout, "radxactl: audio volume -> %u (TODO)\n", vol);
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}

int method_audio_status(sd_bus_message *m, void *u, sd_bus_error *e) {
    fprintf(stdout, "radxactl: audio status (TODO)\n");
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}
