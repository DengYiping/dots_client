//
// Created by Yiping Deng on 4/9/20.
//

#include <coap2/coap.h>
#include "log.h"
#include <stdlib.h>
#include <coap2/coap_dtls.h>
#include "preconditions.h"
#include "signal_channel_handlers.h"
#include "task_env.h"
#include "utils.h"
#include "heartbeat.h"

static coap_dtls_pki_t* load_pki_setup_data() {
    static coap_dtls_pki_t *setup_data = NULL;
    if (setup_data == NULL) {
        dots_client_config *client_context = dots_get_client_config();
        log_info("Loading certificates ...");
        log_info(
                "Using CA %s, public key %s, private key %s",
                client_context->cert_file,
                client_context->client_cert_file,
                client_context->client_key_file);
        setup_data = malloc(sizeof(coap_dtls_pki_t));
        memset(setup_data, 0, sizeof(struct coap_dtls_pki_t));
        // Setup dtls pki configuration
        setup_data->version = COAP_DTLS_PKI_SETUP_VERSION;
        setup_data->pki_key.key_type = COAP_PKI_KEY_PEM;
        setup_data->verify_peer_cert = 1;
        setup_data->require_peer_cert = 1;
        setup_data->allow_self_signed = 1;
        setup_data->allow_expired_certs = 1;
        setup_data->cert_chain_validation = 1;
        setup_data->cert_chain_verify_depth = 2;

        // Use for check that is certificate in certificate revocation list (CRL) from actual server.
        setup_data->check_cert_revocation = 1;
        setup_data->allow_no_crl = 1;
        setup_data->allow_expired_crl = 1;

        setup_data->validate_cn_call_back = NULL;
        setup_data->cn_call_back_arg = NULL;
        setup_data->validate_sni_call_back = NULL;
        setup_data->sni_call_back_arg = NULL;

        coap_pki_key_pem_t *pem = &(setup_data->pki_key.key.pem);
        pem->ca_file = client_context->cert_file;
        pem->public_cert = client_context->client_cert_file;
        pem->private_key = client_context->client_key_file;

    }
    return setup_data;
}

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

static coap_session_t *create_new_psk_session(coap_context_t *ctx) {
    dots_client_config *client_context = dots_get_client_config();
    coap_address_t *addr = resolve_address(client_context->server_addr, client_context->server_port);
    log_info("Making a new psk session!");
    coap_session_t *sess = coap_new_client_session_psk(
            ctx,
            NULL,
            addr,
            COAP_PROTO_DTLS,
            client_context->identity,
            client_context->psk,
            strlen(client_context->psk));
    log_debug("CoAP session is created! Ptr: %p", sess);
    return sess;
}

static coap_session_t *create_new_pki_session(coap_context_t *ctx) {
    dots_client_config *client_context = dots_get_client_config();
    log_info("Making a new pki session!");
    coap_address_t *addr = resolve_address(client_context->server_addr, client_context->server_port);
    coap_session_t *sess = coap_new_client_session_pki(ctx, NULL, addr, COAP_PROTO_DTLS, load_pki_setup_data());

    log_debug("CoAP session is created! Ptr: %p", sess);
    return sess;
}

static int is_using_psk() {
    dots_client_config *client_context = dots_get_client_config();
    int result = client_context->psk != NULL && strlen(client_context->psk) > 0;
    if (result) {
        check_valid(
                client_context->identity,
                "Identity must be present when using PSK!");
    } else {
        check_valid(
                client_context->client_key_file != NULL &&
                client_context->client_cert_file != NULL &&
                client_context->cert_file != NULL,
                "Certficate files are missing!");
    }
    return result;
}

dots_task_env *connect_signal_channel(dots_task_env *old_env) {
    coap_context_t *ctx;
    coap_session_t *sess;
    coap_session_t *o_sess;


    coap_set_show_pdu_output(1);
    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        coap_dtls_set_log_level(LOG_DEBUG);
        coap_set_log_level(LOG_DEBUG);
    }

    if (old_env == NULL) {
        ctx = coap_new_context(NULL);
    } else {
        ctx = old_env->curr_ctx;
    }
    check_valid(ctx != NULL, "Cannot create a CoAP context");

    if (is_using_psk()) {
        create_new_psk_session(ctx);
    } else {
        create_new_pki_session(ctx);
    }

    dots_task_env *env;
    if (old_env == NULL) {
        // Client connection fresh start
        env = dots_new_env(ctx, NULL);
    } else {
        // Client try reconnect
        o_sess = old_env->curr_sess;
        if (o_sess) {
            coap_session_release(o_sess);
        }
        env = old_env;
    }

    dots_set_env(env);
    register_heartbeat_transponder(ctx);
    register_client_handlers(ctx);

    start_heartbeat(env);
    return env;
}
