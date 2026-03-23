#include <stdio.h>
#include <string.h>
#include "daemon/bus.h"
#include "cli/cli.h"

static void print_usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s daemon                     Run as D-Bus service\n"
        "  %s power   <reboot|shutdown|suspend>\n"
        "  %s wifi    <scan|connect|forget|status>\n"
        "  %s bluetooth <enable|disable|scan|status|pair|unpair|\n"
        "  %s            connect|disconnect|discoverable|sink|antenna>\n"
        "  %s audio   <volume <0-100>|status>\n"
        "  %s aic8800 <status|reset|antenna|txpower|region>\n",
        prog, prog, prog, prog, prog, prog, prog);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "daemon") == 0)
        return daemon_run();
    return cli_run(argc - 1, argv + 1);
}