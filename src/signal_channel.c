//
// Created by Yiping Deng on 4/9/20.
//

#include "signal_channel.h"
#include <coap2/coap.h>
#include "log.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include "preconditions.h"
#include "signal_channel_handlers.h"
#include "dots_dtls.h"

static void cleanup_signal_channel(coap_context_t* cxt, coap_session_t* sess) {
    if (cxt) {
        coap_free_context(cxt);
    }
    coap_cleanup();
}
int connect_signal_channel(struct dots_client_context *client_context) {
    coap_context_t* cxt;
    coap_session_t* sess;
    coap_session_t* oSess;
    coap_address_t* addr;
    coap_startup();

    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        coap_dtls_set_log_level(LOG_DEBUG);
        coap_set_log_level(LOG_DEBUG);
    }

    addr = malloc(sizeof(struct coap_address_t));
    memset(addr, 0, sizeof(struct coap_address_t));
    coap_address_init(addr);
    struct sockaddr_in *sin = &(addr->addr.sin);

    // IPv4
    addr->size = sizeof(struct sockaddr_in);
    check_valid(
            inet_pton(AF_INET, client_context->server_addr, sin),
            "Address %s is not a valid IPv4 address!",
            client_context->server_addr);

    sin->sin_port = htons(client_context->server_port);
    sin->sin_family = AF_INET;
    // TODO: IPv6 support

    if (client_context->psk != NULL && strlen(client_context->psk) > 0) {
        check_valid(client_context->identity, "Identity must be present when using PSK!");
        cxt = coap_new_context(addr);
        check_valid(cxt != NULL, "Cannot create a CoAP context");

        sess = coap_new_client_session_psk(
                cxt,
                NULL,
                addr,
                COAP_PROTO_DTLS,
                client_context->identity,
                client_context->psk,
                strlen(client_context->psk));
    } else {
        panic("Asymmetric encryption is not supported at the moment!");
    }

    // Create resource for heartbeat mechanism from server
    coap_resource_t* heartbeat_resource = coap_resource_unknown_init(heartbeat_handler);
    check_valid(heartbeat_resource, "Heartbeat resource cannot be created!");
    coap_add_resource(cxt, heartbeat_resource);

    return 1;
}
