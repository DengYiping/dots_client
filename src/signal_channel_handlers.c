//
// Created by Yiping Deng on 4/11/20.
//

#include "signal_channel_handlers.h"
#include "log.h"
#include "heartbeat.h"
#include "dots_code.h"
#include "preconditions.h"

int connect_signal_channel(dots_task_env *old_env);

static const char *const ERROR_MSG = "Invalid heartbeat!";

static dots_task_env *curr_env = NULL;

static dots_task_env *org_env = NULL;
static struct coap_session_t *new_sess = NULL;
static struct coap_session_t *o_sess = NULL;

void dots_set_env(dots_task_env *env) {
    curr_env = env;
}

static void handle_response(dots_task_env *env, coap_pdu_t *pdu) {
    char map_key[64];
    memset(map_key, 0, sizeof(map_key));
    sprintf(map_key, "%d", pdu->tid);
    // If it is a heartbeat response

    if (COAP_PDU_IS_RESPONSE(pdu)) {
        receive_dots_response_callback *callback_ptr = map_get(&env->pending_request_map, map_key);
        if (callback_ptr != NULL) {
            receive_dots_response_callback callback = *callback_ptr;
            map_remove(&env->pending_request_map, map_key);
            callback(pdu, env);
        } else {
            log_warn("Dangling request response with message id %s", map_key);
        }
    }
}

static void handle_request_timeout(dots_task_env *env, coap_pdu_t *pdu) {

}


/**
 * Handle active heartbeat requests send by the server
 * @param ctx
 * @param resource
 * @param sess
 * @param request
 * @param token
 * @param query
 * @param response
 */
void heartbeat_handler(
        coap_context_t *ctx,
        struct coap_resource_t *resource,
        coap_session_t *sess,
        coap_pdu_t *request,
        coap_binary_t *token,
        coap_string_t *query,
        coap_pdu_t *response) {
    log_debug("Handle receive heartbeat from server.");

    size_t data_len;
    uint8_t *data;
    coap_get_data(request, &data_len, &data);

    // MessageId
    response->tid = request->tid;
    if (!validate_cbor_heartbeat_body(data, data_len)) {
        log_warn("Heartbeat failure has been reported by the server! This might indicates a connection issue!");
        response->code = ResponseBadRequest;
        response->type = COAP_MESSAGE_NON; // TypeNon
        coap_add_data(response, strlen(ERROR_MSG), ERROR_MSG);
    } else {
        log_debug("Successful heartbeat response!");
        response->code = ResponseChanged;
        response->type = COAP_MESSAGE_NON;
    }

    // TODO: setIsReceivedHeartBeat
}


/**
 * Handle events like DTLS connected and closed.
 * @param ctx
 * @param event
 * @param sess
 */
void event_handler(struct coap_context_t *ctx,
                   coap_event_t event,
                   struct coap_session_t *sess) {
    /*
    if (event == COAP_EVENT_DTLS_CONNECTED) {
        if (org_env) {
            org_env->replacing_sess = sess;
        }
    } else if (event == COAP_EVENT_DTLS_CLOSED) {
        coap_session_release(sess);
        restart_connection(curr_env);
    }
     */
    switch (event) {
        case COAP_EVENT_DTLS_CONNECTED:
            log_info("Connection is successfully established on session! Ptr: %p", sess);
            if (curr_env->curr_sess != sess) {
                dots_renew_env_with_session(curr_env, sess);
            }
            break;
        case COAP_EVENT_DTLS_RENEGOTIATE:
            log_info("Connection is renegotiated on session! Ptr: %p", sess);
            break;
        case COAP_EVENT_DTLS_CLOSED:
            log_info("Connection is closed on session! Ptr: %p", sess);
            if (curr_env->curr_sess == NULL || curr_env->curr_sess == sess) {
                curr_env->curr_sess = NULL;
            }
            break;
        case COAP_EVENT_DTLS_ERROR:
            log_info("Received a CoAP error event!");
            break;
        default:
            panic("Unsupported event: %i", event);
    }
}

void response_handler(struct coap_context_t *context,
                      coap_session_t *session,
                      coap_pdu_t *sent,
                      coap_pdu_t *received,
                      const coap_tid_t id) {
    log_debug("New response received!");
    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        printf("-----PDU-RESPONSE----\n");
        coap_show_pdu(LOG_DEBUG, received);
        printf("-----PDU-RESPONSE----\n");
    }

    handle_response(curr_env, received);
    /*
    if (received != NULL && o_sess != NULL && o_sess == curr_env->curr_sess) {
        // TODO: handle restarted session
    }
    */
}

void nack_handler(struct coap_context_t *context,
                  coap_session_t *session,
                  coap_pdu_t *sent,
                  coap_nack_reason_t reason,
                  const coap_tid_t id) {
    log_debug("New NACK received!");
    if (reason == COAP_NACK_RST) {
        // Pong message
        handle_response(curr_env, sent);
    } else if (reason == COAP_NACK_TOO_MANY_RETRIES) {
        // Ping timeout
        handle_request_timeout(curr_env, sent);
    } else {
        log_error("nack_handler gets fired with unsupported reason type!");
    }
}