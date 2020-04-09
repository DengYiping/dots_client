#include "log.h"
#include "argparse.h"
#include <stdlib.h>

static const char *const usage[] = {
        "dots_client [options] [[--] args]",
        "dots_client [options]",
        NULL,
};

#define DEBUG_MODE (1<<0)

int main(int argc, char **argv) {
    const char *server_addr = "127.0.0.1";
    int server_port = -1;
    int flags = 0;

    // Argument parsing
    struct argparse_option options[] = {
            OPT_HELP(),
            OPT_STRING('s', "server", &server_addr, "server address"),
            OPT_INTEGER('p', "port", &server_port, "server port number"),
            OPT_BIT(0, "debug", &flags, "debug mode", NULL, DEBUG_MODE, OPT_NONEG),
            OPT_END()
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(
            &argparse,
            "\nA DOTS protocol client program.",
            "It can be used to signal ongoing DDoS attack!");
    argc = argparse_parse(&argparse, argc, argv);

    // Arguments validation
    if (server_port == -1) {
        log_error("Server port is missing as arguments!");
        exit(EXIT_FAILURE);
    }

    if (DEBUG_MODE & flags) {
        log_set_level(LOG_DEBUG);
    } else {
        log_set_level(LOG_INFO);
    }

    log_info("Server address: %s", server_addr);
    return 0;
}
