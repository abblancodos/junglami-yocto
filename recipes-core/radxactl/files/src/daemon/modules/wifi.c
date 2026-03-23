#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "wifi.h"
#include "../bus.h"

/*
 * TODO: implementar llamadas a org.freedesktop.NetworkManager via D-Bus
 *
 * WifiScan:    org.freedesktop.NetworkManager.Device.Wireless.RequestScan
 * WifiConnect: crear connection profile + ActivateConnection
 * WifiForget:  org.freedesktop.NetworkManager.Settings.Connection.Delete
 * WifiStatus:  leer state de org.freedesktop.NetworkManager
 *
 * Suscribirse a org.freedesktop.NetworkManager.StateChanged para
 * emitir WifiStateChanged hacia el UI sin polling.
 */

int method_wifi_scan(sd_bus_message *m, void *u, sd_bus_error *e) {
    fprintf(stdout, "radxactl: wifi scan (TODO)\n");
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}

int method_wifi_connect(sd_bus_message *m, void *u, sd_bus_error *e) {
    const char *ssid = NULL, *pass = NULL;
    int r = sd_bus_message_read(m, "ss", &ssid, &pass);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid arguments");
    fprintf(stdout, "radxactl: wifi connect ssid=%s (TODO)\n", ssid);
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}

int method_wifi_forget(sd_bus_message *m, void *u, sd_bus_error *e) {
    const char *ssid = NULL;
    int r = sd_bus_message_read(m, "s", &ssid);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    fprintf(stdout, "radxactl: wifi forget ssid=%s (TODO)\n", ssid);
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}

int method_wifi_status(sd_bus_message *m, void *u, sd_bus_error *e) {
    fprintf(stdout, "radxactl: wifi status (TODO)\n");
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}