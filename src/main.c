#include "log.h"
#include "argparse.h"
#include <stdlib.h>
#include <string.h>
#include "client_context.h"
#include "signal_channel.h"
#include "preconditions.h"

static const char *const usage[] = {
        "dots_client [options] [[--] args]",
        "dots_client [options]",
        NULL,
};

#define DEBUG_MODE (1<<0)

int main(int argc, char **argv) {
    dots_client_context *client_context = malloc(sizeof(dots_client_context));
    memset(client_context, 0, sizeof(dots_client_context));
    client_context->server_addr = "127.0.0.1";
    client_context->server_port = -1;

    int flags = 0;

    // Argument parsing
    struct argparse_option options[] = {
            OPT_HELP(),
            OPT_STRING('s', "server", &(client_context->server_addr), "server address"),
            OPT_INTEGER('p', "port", &(client_context->server_port), "server port number"),
            OPT_BIT(0, "debug", &flags, "debug mode", NULL, DEBUG_MODE, OPT_NONEG),
            OPT_GROUP("PSK"),
            OPT_STRING(0, "psk", &(client_context->psk), "DTLS PSK"),
            OPT_STRING(0, "identity", &(client_context->identity), "DTLS identity"),
            OPT_GROUP("Certificate"),
            OPT_STRING(0, "cert_file", &(client_context->cert_file), "Certification file"),
            OPT_STRING(0, "client_cert_file", &(client_context->client_cert_file), "Certification file for client"),
            OPT_STRING(0, "client_key_file", &(client_context->client_key_file), "Key file used by the client"),
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
    check_valid(client_context->server_port != -1, "Server port is missing as arguments!");

    if (DEBUG_MODE & flags) {
        log_set_level(LOG_LEVEL_DEBUG);
    } else {
        log_set_level(LOG_LEVEL_INFO);
    }

    log_info("Server address: %s", client_context->server_addr);
    connect_signal_channel(client_context);

    free(client_context);
    return 0;
}
