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
#include "utils.h"

static void cleanup_signal_channel(coap_context_t *cxt, coap_session_t *sess) {
    if (sess) {
        coap_session_release(sess);
    }
    if (cxt) {
        coap_free_context(cxt);
    }
    coap_cleanup();
}

static void register_heartbeat_transponder(coap_context_t *ctx) {
    // Create resource for heartbeat mechanism from server
    coap_resource_t *heartbeat_resource = coap_resource_unknown_init(NULL);
    check_valid(heartbeat_resource, "Heartbeat resource cannot be created!");
    coap_add_resource(ctx, heartbeat_resource);
    coap_register_handler(heartbeat_resource, COAP_REQUEST_PUT, heartbeat_handler);
}

static void register_client_handlers(coap_context_t *ctx) {
    // Handle new connection established & disconnect event
    coap_register_event_handler(ctx, event_handler);
    coap_register_response_handler(ctx, response_handler);
    coap_register_nack_handler(ctx, nack_handler);
}

dots_task_env *connect_signal_channel(dots_task_env *org_env) {
    coap_context_t *ctx;
    coap_session_t *sess;
    // coap_session_t *o_sess;
    coap_address_t *addr;

    dots_client_config *client_context = dots_get_client_config();
    coap_startup();

    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        coap_dtls_set_log_level(LOG_DEBUG);
        coap_set_log_level(LOG_DEBUG);
    }

    addr = resolve_address(client_context->server_addr, client_context->server_port);

    if (client_context->psk != NULL && strlen(client_context->psk) > 0) {
        check_valid(client_context->identity, "Identity must be present when using PSK!");
        ctx = coap_new_context(NULL);
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
        log_info("New CoAP session is created!");
    } else {
        panic("Asymmetric encryption is not supported at the moment!");
    }

    // Environment for handler callbacks
    /*
    dots_task_env *env;
    if (org_env == NULL) {
        env = dots_new_env(ctx, sess);
    } else {
        o_sess = org_env->curr_sess;
        env = org_env;
    }
     */

    dots_task_env *env = dots_new_env(ctx, sess);
    dots_set_env(env);
    /*
    dots_set_org_env(org_env);
    dots_set_o_sess(o_sess);
    dots_set_new_sess(sess);
     */
    register_heartbeat_transponder(ctx);
    register_client_handlers(ctx);

    return env;
}
