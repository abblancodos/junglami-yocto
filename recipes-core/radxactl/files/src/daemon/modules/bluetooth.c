#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include "bluetooth.h"
#include "../bus.h"

/*
 * Helpers para llamar a org.bluez via D-Bus
 *
 * Adapter path: /org/bluez/hci0
 * Device path:  /org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX
 *               (reemplazar : por _ en la MAC)
 */

#define BLUEZ_BUS        "org.bluez"
#define BLUEZ_ADAPTER    "/org/bluez/hci0"
#define BLUEZ_ADAPTER_IF "org.bluez.Adapter1"
#define BLUEZ_DEVICE_IF  "org.bluez.Device1"
#define BLUEZ_AGENT_MGR  "org.bluez.AgentManager1"
#define BLUEZ_AGENT_PATH "/cr/junglami/radxactl/agent"

/* Convierte "AA:BB:CC:DD:EE:FF" → "/org/bluez/hci0/dev_AA_BB_CC_DD_EE_FF" */
static void mac_to_dbus_path(const char *mac, char *out, size_t len) {
    snprintf(out, len, "/org/bluez/hci0/dev_");
    size_t base = strlen(out);
    for (int i = 0; mac[i] && base < len - 1; i++, base++) {
        out[base] = (mac[i] == ':') ? '_' : mac[i];
    }
    out[base] = '\0';
}

/* Llama un método void en el adapter */
static int adapter_call(sd_bus_message *m, sd_bus_error *err,
                        const char *method) {
    sd_bus_message *reply = NULL;
    int r = sd_bus_call_method(bus_get(),
            BLUEZ_BUS, BLUEZ_ADAPTER, BLUEZ_ADAPTER_IF,
            method, err, &reply, "");
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: bluez %s failed: %s\n",
                method, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "bluez call failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

/* Setea una property booleana en el adapter */
static int adapter_set_bool(sd_bus_message *m, sd_bus_error *err,
                            const char *property, int value) {
    int r = sd_bus_set_property(bus_get(),
            BLUEZ_BUS, BLUEZ_ADAPTER, BLUEZ_ADAPTER_IF,
            property, err, "b", value);
    if (r < 0) {
        fprintf(stderr, "radxactl: set %s failed: %s\n",
                property, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "property set failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_bt_enable(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: bluetooth enable\n");
    return adapter_set_bool(m, err, "Powered", 1);
}

int method_bt_disable(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: bluetooth disable\n");
    return adapter_set_bool(m, err, "Powered", 0);
}

int method_bt_set_discoverable(sd_bus_message *m, void *u, sd_bus_error *err) {
    int enable = 0;
    int r = sd_bus_message_read(m, "b", &enable);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    fprintf(stdout, "radxactl: bluetooth discoverable -> %s\n",
            enable ? "on" : "off");
    return adapter_set_bool(m, err, "Discoverable", enable);
}

int method_bt_scan(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: bluetooth scan start\n");
    return adapter_call(m, err, "StartDiscovery");
}

int method_bt_status(sd_bus_message *m, void *u, sd_bus_error *err) {
    int powered = 0, discoverable = 0;
    char status[128];

    sd_bus_get_property_trivial(bus_get(),
            BLUEZ_BUS, BLUEZ_ADAPTER, BLUEZ_ADAPTER_IF,
            "Powered", NULL, 'b', &powered);
    sd_bus_get_property_trivial(bus_get(),
            BLUEZ_BUS, BLUEZ_ADAPTER, BLUEZ_ADAPTER_IF,
            "Discoverable", NULL, 'b', &discoverable);

    snprintf(status, sizeof(status),
             "powered=%s discoverable=%s",
             powered ? "yes" : "no",
             discoverable ? "yes" : "no");

    return sd_bus_reply_method_return(m, "bs", 1, status);
}

int method_bt_pair(sd_bus_message *m, void *u, sd_bus_error *err) {
    const char *mac = NULL;
    int r = sd_bus_message_read(m, "s", &mac);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    char dev_path[64];
    mac_to_dbus_path(mac, dev_path, sizeof(dev_path));

    fprintf(stdout, "radxactl: bluetooth pair %s -> %s\n", mac, dev_path);

    /*
     * Pair sin código: registrar un NoInputNoOutput agent primero.
     * Por ahora llamamos Pair directamente — BlueZ usará el agent
     * default si está registrado, o fallará si requiere PIN.
     * TODO: registrar agent NoInputNoOutput en daemon_run()
     */
    sd_bus_message *reply = NULL;
    r = sd_bus_call_method(bus_get(),
            BLUEZ_BUS, dev_path, BLUEZ_DEVICE_IF,
            "Pair", err, &reply, "");
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: pair %s failed: %s\n",
                mac, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "pair failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_bt_unpair(sd_bus_message *m, void *u, sd_bus_error *err) {
    const char *mac = NULL;
    int r = sd_bus_message_read(m, "s", &mac);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    char dev_path[64];
    mac_to_dbus_path(mac, dev_path, sizeof(dev_path));

    fprintf(stdout, "radxactl: bluetooth unpair %s\n", mac);

    sd_bus_message *reply = NULL;
    r = sd_bus_call_method(bus_get(),
            BLUEZ_BUS, BLUEZ_ADAPTER, BLUEZ_ADAPTER_IF,
            "RemoveDevice", err, &reply, "o", dev_path);
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: unpair %s failed: %s\n",
                mac, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "unpair failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_bt_connect(sd_bus_message *m, void *u, sd_bus_error *err) {
    const char *mac = NULL;
    int r = sd_bus_message_read(m, "s", &mac);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    char dev_path[64];
    mac_to_dbus_path(mac, dev_path, sizeof(dev_path));

    fprintf(stdout, "radxactl: bluetooth connect %s\n", mac);

    sd_bus_message *reply = NULL;
    r = sd_bus_call_method(bus_get(),
            BLUEZ_BUS, dev_path, BLUEZ_DEVICE_IF,
            "Connect", err, &reply, "");
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: connect %s failed: %s\n",
                mac, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "connect failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_bt_disconnect(sd_bus_message *m, void *u, sd_bus_error *err) {
    const char *mac = NULL;
    int r = sd_bus_message_read(m, "s", &mac);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    char dev_path[64];
    mac_to_dbus_path(mac, dev_path, sizeof(dev_path));

    fprintf(stdout, "radxactl: bluetooth disconnect %s\n", mac);

    sd_bus_message *reply = NULL;
    r = sd_bus_call_method(bus_get(),
            BLUEZ_BUS, dev_path, BLUEZ_DEVICE_IF,
            "Disconnect", err, &reply, "");
    sd_bus_message_unref(reply);
    if (r < 0) {
        fprintf(stderr, "radxactl: disconnect %s failed: %s\n",
                mac, err->message ? err->message : strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0,
                err->message ? err->message : "disconnect failed");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_bt_set_sink_mode(sd_bus_message *m, void *u, sd_bus_error *err) {
    int enable = 0;
    int r = sd_bus_message_read(m, "b", &enable);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    fprintf(stdout, "radxactl: bluetooth sink mode -> %s (TODO: wireplumber)\n",
            enable ? "on" : "off");
    /*
     * TODO: escribir config WirePlumber y reiniciar wireplumber
     * echo '["bluez5.roles"] = "[ a2dp_sink a2dp_source ]"' >
     *     /etc/wireplumber/bluetooth.lua.d/50-roles.lua
     * systemctl restart wireplumber
     */
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}

int method_bt_set_antenna(sd_bus_message *m, void *u, sd_bus_error *err) {
    unsigned int antenna = 0;
    int r = sd_bus_message_read(m, "u", &antenna);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    if (antenna > 1)
        return sd_bus_reply_method_return(m, "bs", 0, "antenna must be 0 (internal) or 1 (external)");
    fprintf(stdout, "radxactl: bluetooth antenna -> %s (TODO: AIC8800 sysfs)\n",
            antenna == 0 ? "internal" : "external");
    /*
     * TODO: investigar mecanismo AIC8800 para cambio de antena
     * Probablemente via sysfs o debugfs del driver aic8800
     */
    return sd_bus_reply_method_return(m, "bs", 0, "not implemented yet");
}