#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <systemd/sd-bus.h>
#include "cli.h"
#include "../daemon/bus.h"

static int call_method(sd_bus *bus, const char *method,
                       const char *arg_types, ...) {
    sd_bus_error err = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;
    int ok = 0;
    const char *msg = NULL;
    int r;
    va_list ap;
    va_start(ap, arg_types);
    r = sd_bus_call_methodv(bus,
            RADXACTL_BUS_NAME, RADXACTL_OBJECT_PATH, RADXACTL_INTERFACE,
            method, &err, &reply, arg_types, ap);
    va_end(ap);
    if (r < 0) {
        fprintf(stderr, "error: %s\n", err.message ? err.message : strerror(-r));
        sd_bus_error_free(&err);
        return 1;
    }
    r = sd_bus_message_read(reply, "bs", &ok, &msg);
    if (r < 0)
        fprintf(stderr, "error reading reply: %s\n", strerror(-r));
    else
        printf("%s: %s\n", ok ? "ok" : "error", msg ? msg : "");
    sd_bus_message_unref(reply);
    sd_bus_error_free(&err);
    return ok ? 0 : 1;
}

static int cmd_power(sd_bus *bus, int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "usage: radxactl power <reboot|shutdown|suspend>\n");
        return 1;
    }
    if (strcmp(argv[0], "reboot")   == 0) return call_method(bus, "Reboot",   "", NULL);
    if (strcmp(argv[0], "shutdown") == 0) return call_method(bus, "Shutdown", "", NULL);
    if (strcmp(argv[0], "suspend")  == 0) return call_method(bus, "Suspend",  "", NULL);
    fprintf(stderr, "unknown: %s\n", argv[0]);
    return 1;
}

static int cmd_bluetooth(sd_bus *bus, int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr,
            "usage: radxactl bluetooth <command>\n"
            "  enable\n"
            "  disable\n"
            "  scan\n"
            "  status\n"
            "  pair <mac>\n"
            "  unpair <mac>\n"
            "  connect <mac>\n"
            "  disconnect <mac>\n"
            "  discoverable <on|off>\n"
            "  sink <on|off>\n");
        return 1;
    }
    if (strcmp(argv[0], "enable")  == 0) return call_method(bus, "BluetoothEnable",  "", NULL);
    if (strcmp(argv[0], "disable") == 0) return call_method(bus, "BluetoothDisable", "", NULL);
    if (strcmp(argv[0], "scan")    == 0) return call_method(bus, "BluetoothScan",    "", NULL);
    if (strcmp(argv[0], "status")  == 0) return call_method(bus, "BluetoothStatus",  "", NULL);
    if (strcmp(argv[0], "pair") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth pair <mac>\n"); return 1; }
        return call_method(bus, "BluetoothPair", "s", argv[1]);
    }
    if (strcmp(argv[0], "unpair") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth unpair <mac>\n"); return 1; }
        return call_method(bus, "BluetoothUnpair", "s", argv[1]);
    }
    if (strcmp(argv[0], "connect") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth connect <mac>\n"); return 1; }
        return call_method(bus, "BluetoothConnect", "s", argv[1]);
    }
    if (strcmp(argv[0], "disconnect") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth disconnect <mac>\n"); return 1; }
        return call_method(bus, "BluetoothDisconnect", "s", argv[1]);
    }
    if (strcmp(argv[0], "discoverable") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth discoverable <on|off>\n"); return 1; }
        int on = strcmp(argv[1], "on") == 0 ? 1 : 0;
        return call_method(bus, "BluetoothSetDiscoverable", "b", on);
    }
    if (strcmp(argv[0], "sink") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl bluetooth sink <on|off>\n"); return 1; }
        int on = strcmp(argv[1], "on") == 0 ? 1 : 0;
        return call_method(bus, "BluetoothSetSinkMode", "b", on);
    }
    fprintf(stderr, "unknown bluetooth command: %s\n", argv[0]);
    return 1;
}

static int cmd_wifi(sd_bus *bus, int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr,
            "usage: radxactl wifi <command>\n"
            "  scan\n"
            "  status\n"
            "  connect <ssid> <password>\n"
            "  forget <ssid>\n");
        return 1;
    }
    if (strcmp(argv[0], "scan")    == 0) return call_method(bus, "WifiScan",   "", NULL);
    if (strcmp(argv[0], "status")  == 0) return call_method(bus, "WifiStatus", "", NULL);
    if (strcmp(argv[0], "forget")  == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl wifi forget <ssid>\n"); return 1; }
        return call_method(bus, "WifiForget", "s", argv[1]);
    }
    if (strcmp(argv[0], "connect") == 0) {
        if (argc < 3) { fprintf(stderr, "usage: radxactl wifi connect <ssid> <password>\n"); return 1; }
        return call_method(bus, "WifiConnect", "ss", argv[1], argv[2]);
    }
    fprintf(stderr, "unknown wifi command: %s\n", argv[0]);
    return 1;
}

static int cmd_audio(sd_bus *bus, int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr,
            "usage: radxactl audio <command>\n"
            "  status\n"
            "  volume <0-100>\n");
        return 1;
    }
    if (strcmp(argv[0], "status") == 0) return call_method(bus, "AudioStatus", "", NULL);
    if (strcmp(argv[0], "volume") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl audio volume <0-100>\n"); return 1; }
        unsigned int vol = (unsigned int)atoi(argv[1]);
        if (vol > 100) { fprintf(stderr, "volume must be 0-100\n"); return 1; }
        return call_method(bus, "AudioSetVolume", "u", vol);
    }
    fprintf(stderr, "unknown audio command: %s\n", argv[0]);
    return 1;
}

static int cmd_aic8800(sd_bus *bus, int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr,
            "usage: radxactl aic8800 <command>\n"
            "  status\n"
            "  reset\n"
            "  antenna <0|1>             0=internal 1=external\n"
            "  txpower <dBm>             potencia de transmisión\n"
            "  region  <CR|EU|US|JP|CN>\n");
        return 1;
    }
    if (strcmp(argv[0], "status") == 0) return call_method(bus, "AIC8800Status", "", NULL);
    if (strcmp(argv[0], "reset")  == 0) return call_method(bus, "AIC8800Reset",  "", NULL);
    if (strcmp(argv[0], "antenna") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl aic8800 antenna <0|1>\n"); return 1; }
        unsigned int ant = (unsigned int)atoi(argv[1]);
        return call_method(bus, "AIC8800SetAntenna", "u", ant);
    }
    if (strcmp(argv[0], "txpower") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl aic8800 txpower <dBm>\n"); return 1; }
        unsigned int dbm = (unsigned int)atoi(argv[1]);
        return call_method(bus, "AIC8800SetTxPower", "u", dbm);
    }
    if (strcmp(argv[0], "region") == 0) {
        if (argc < 2) { fprintf(stderr, "usage: radxactl aic8800 region <CR|EU|US|JP|CN>\n"); return 1; }
        return call_method(bus, "AIC8800SetRegion", "s", argv[1]);
    }
    fprintf(stderr, "unknown aic8800 command: %s\n", argv[0]);
    return 1;
}

int cli_run(int argc, char *argv[]) {
    sd_bus *bus = NULL;
    int r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "error: cannot connect to system bus: %s\n", strerror(-r));
        return 1;
    }
    int ret = 1;
    if      (strcmp(argv[0], "power")     == 0) ret = cmd_power(bus,     argc-1, argv+1);
    else if (strcmp(argv[0], "bluetooth") == 0) ret = cmd_bluetooth(bus, argc-1, argv+1);
    else if (strcmp(argv[0], "wifi")      == 0) ret = cmd_wifi(bus,      argc-1, argv+1);
    else if (strcmp(argv[0], "audio")     == 0) ret = cmd_audio(bus,     argc-1, argv+1);
    else if (strcmp(argv[0], "aic8800")   == 0) ret = cmd_aic8800(bus,   argc-1, argv+1);
    else {
        fprintf(stderr, "unknown module: %s\n", argv[0]);
        fprintf(stderr, "modules: power, bluetooth, wifi, audio, aic8800\n");
    }
    sd_bus_unref(bus);
    return ret;
}