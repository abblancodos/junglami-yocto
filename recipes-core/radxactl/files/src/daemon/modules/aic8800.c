#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <systemd/sd-bus.h>
#include "aic8800.h"
#include "../bus.h"

/*
 * El driver AIC8800 expone su interfaz via sysfs/debugfs.
 * Paths típicos — confirmar en la imagen con:
 *   find /sys /proc -name "*aic*" 2>/dev/null
 *   find /sys/kernel/debug -name "*aic*" 2>/dev/null
 *
 * Los paths de abajo son candidatos basados en el driver aic8800-driver.
 * TODO: verificar paths reales en la Radxa Zero 3W.
 */

#define AIC_SYSFS_BASE      "/sys/module/aic8800_fdrv/parameters"
#define AIC_TXPOWER_PATH    AIC_SYSFS_BASE "/txpwr_lvl"
#define AIC_ANTENNA_PATH    AIC_SYSFS_BASE "/ant_sel"
#define AIC_REGION_PATH     AIC_SYSFS_BASE "/country_code"
#define AIC_FW_VER_PATH     "/sys/module/aic8800_fdrv/version"

/*
 * Límites de potencia por región (dBm).
 * Basado en regulaciones IEEE 802.11 estándar.
 * CR sigue ETSI (igual que EU).
 */
typedef struct {
    const char *code;
    int max_txpower_dbm;
} region_limit_t;

static const region_limit_t region_limits[] = {
    { "CR", 20 },   /* Costa Rica — sigue ETSI */
    { "EU", 20 },   /* Europa ETSI */
    { "US", 30 },   /* FCC */
    { "JP", 23 },   /* Japan */
    { "CN", 23 },   /* China */
    { NULL, 0  }
};

static int get_max_txpower(const char *region) {
    for (int i = 0; region_limits[i].code != NULL; i++) {
        if (strcmp(region_limits[i].code, region) == 0)
            return region_limits[i].max_txpower_dbm;
    }
    return 20; /* default conservador si región desconocida */
}

/* Lee un valor string de sysfs */
static int sysfs_read(const char *path, char *buf, size_t len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -errno;
    ssize_t n = read(fd, buf, len - 1);
    close(fd);
    if (n < 0) return -errno;
    buf[n] = '\0';
    /* quitar newline */
    if (n > 0 && buf[n-1] == '\n') buf[n-1] = '\0';
    return 0;
}

/* Escribe un valor string a sysfs */
static int sysfs_write(const char *path, const char *val) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) return -errno;
    ssize_t n = write(fd, val, strlen(val));
    close(fd);
    if (n < 0) return -errno;
    return 0;
}

int method_aic8800_set_antenna(sd_bus_message *m, void *u, sd_bus_error *err) {
    unsigned int ant = 0;
    int r = sd_bus_message_read(m, "u", &ant);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");
    if (ant > 1)
        return sd_bus_reply_method_return(m, "bs", 0, "antenna must be 0 (internal) or 1 (external)");

    fprintf(stdout, "radxactl: aic8800 antenna -> %s\n",
            ant == 0 ? "internal" : "external");

    char val[4];
    snprintf(val, sizeof(val), "%u", ant);
    r = sysfs_write(AIC_ANTENNA_PATH, val);
    if (r < 0) {
        fprintf(stderr, "radxactl: aic8800 antenna sysfs write failed: %s\n", strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0, "sysfs write failed (TODO: verify path)");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_aic8800_reset(sd_bus_message *m, void *u, sd_bus_error *err) {
    fprintf(stdout, "radxactl: aic8800 reset\n");
    /*
     * TODO: investigar si el driver expone un mecanismo de reset.
     * Opciones:
     *   1. rmmod + modprobe del driver
     *   2. Escribir a un sysfs trigger si existe
     *   3. Usar rfkill block/unblock
     *
     * Por ahora usamos rfkill como aproximación.
     */
    int r = system("rfkill block wifi && rfkill block bluetooth && "
                   "sleep 1 && "
                   "rfkill unblock wifi && rfkill unblock bluetooth");
    if (r != 0)
        return sd_bus_reply_method_return(m, "bs", 0, "rfkill reset failed");
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_aic8800_status(sd_bus_message *m, void *u, sd_bus_error *err) {
    char fw_ver[64]   = "unknown";
    char antenna[16]  = "unknown";
    char region[8]    = "unknown";
    char txpower[16]  = "unknown";
    char status[256];

    sysfs_read(AIC_FW_VER_PATH,   fw_ver,  sizeof(fw_ver));
    sysfs_read(AIC_ANTENNA_PATH,  antenna, sizeof(antenna));
    sysfs_read(AIC_REGION_PATH,   region,  sizeof(region));
    sysfs_read(AIC_TXPOWER_PATH,  txpower, sizeof(txpower));

    /* Verificar si la potencia actual viola el límite de región */
    const char *regulatory = "ok";
    if (strcmp(region, "unknown") != 0 && strcmp(txpower, "unknown") != 0) {
        int max = get_max_txpower(region);
        int current = atoi(txpower);
        if (current > max)
            regulatory = "VIOLATION";
    }

    snprintf(status, sizeof(status),
             "firmware=%s antenna=%s region=%s txpower=%sdBm regulatory=%s",
             fw_ver,
             strcmp(antenna, "0") == 0 ? "internal" :
             strcmp(antenna, "1") == 0 ? "external" : antenna,
             region, txpower, regulatory);

    return sd_bus_reply_method_return(m, "bs", 1, status);
}

int method_aic8800_set_txpower(sd_bus_message *m, void *u, sd_bus_error *err) {
    unsigned int dbm = 0;
    int r = sd_bus_message_read(m, "u", &dbm);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    /* Leer región actual para verificar límite */
    char region[8] = "unknown";
    sysfs_read(AIC_REGION_PATH, region, sizeof(region));

    if (strcmp(region, "unknown") != 0) {
        int max = get_max_txpower(region);
        if ((int)dbm > max) {
            char warn[128];
            snprintf(warn, sizeof(warn),
                     "txpower %udBm exceeds regulatory limit %ddBm for region %s",
                     dbm, max, region);
            fprintf(stderr, "radxactl: WARNING: %s\n", warn);
            return sd_bus_reply_method_return(m, "bs", 0, warn);
        }
    }

    fprintf(stdout, "radxactl: aic8800 txpower -> %udBm\n", dbm);

    char val[16];
    snprintf(val, sizeof(val), "%u", dbm);
    r = sysfs_write(AIC_TXPOWER_PATH, val);
    if (r < 0) {
        fprintf(stderr, "radxactl: aic8800 txpower sysfs write failed: %s\n", strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0, "sysfs write failed (TODO: verify path)");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}

int method_aic8800_set_region(sd_bus_message *m, void *u, sd_bus_error *err) {
    const char *region = NULL;
    int r = sd_bus_message_read(m, "s", &region);
    if (r < 0)
        return sd_bus_reply_method_return(m, "bs", 0, "invalid argument");

    /* Verificar que la región es conocida */
    int known = 0;
    for (int i = 0; region_limits[i].code != NULL; i++) {
        if (strcmp(region_limits[i].code, region) == 0) {
            known = 1;
            break;
        }
    }
    if (!known) {
        char msg[64];
        snprintf(msg, sizeof(msg), "unknown region '%s' — use CR, EU, US, JP, CN", region);
        return sd_bus_reply_method_return(m, "bs", 0, msg);
    }

    fprintf(stdout, "radxactl: aic8800 region -> %s (max %ddBm)\n",
            region, get_max_txpower(region));

    r = sysfs_write(AIC_REGION_PATH, region);
    if (r < 0) {
        fprintf(stderr, "radxactl: aic8800 region sysfs write failed: %s\n", strerror(-r));
        return sd_bus_reply_method_return(m, "bs", 0, "sysfs write failed (TODO: verify path)");
    }
    return sd_bus_reply_method_return(m, "bs", 1, "ok");
}