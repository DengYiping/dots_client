//
// Created by Yiping Deng on 4/9/20.
//

#include <coap2/coap.h>
#include "log.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include "preconditions.h"
#include "signal_channel_handlers.h"
#include "task_env.h"

static void cleanup_signal_channel(coap_context_t *cxt, coap_session_t *sess) {
    if (sess) {
        coap_session_release(sess);
    }
    if (cxt) {
        coap_free_context(cxt);
    }
    coap_cleanup();
}

dots_task_env *connect_signal_channel(dots_task_env *org_env) {
    coap_context_t *ctx;
    coap_session_t *sess;

    // coap_session_t *o_sess;

    coap_address_t *addr;

    coap_startup();

    dots_client_context *client_context = dots_get_client_context();

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
        ctx = coap_new_context(addr);
        check_valid(ctx != NULL, "Cannot create a CoAP context");

        log_info("Making a new session!");
        sess = coap_new_client_session_psk(
                ctx,
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
    coap_resource_t *heartbeat_resource = coap_resource_unknown_init(heartbeat_handler);
    check_valid(heartbeat_resource, "Heartbeat resource cannot be created!");
    coap_add_resource(ctx, heartbeat_resource);

    // Handle reconnection
    /*
    dots_task_env *env;
    if (org_env == NULL) {
        env = dots_new_env(ctx, sess);
    } else {
        o_sess = org_env->curr_sess;
        env = org_env;
    }
     */

    dots_task_env* env = dots_new_env(ctx, sess);

    dots_set_env(env);

    /*
    dots_set_org_env(org_env);
    dots_set_o_sess(o_sess);
    dots_set_new_sess(sess);
     */

    // Handle new connection established & disconnect event
    coap_register_event_handler(ctx, event_handler);
    coap_register_response_handler(ctx, response_handler);
    coap_register_nack_handler(ctx, nack_handler);
    return env;
}
