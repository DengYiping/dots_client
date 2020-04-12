//
// Created by Yiping Deng on 4/11/20.
//

#include "signal_channel_handlers.h"
#include "log.h"
#include "heartbeat.h"
#include "dots_code.h"
#include "preconditions.h"

int connect_signal_channel(dots_task_env *org_env);
static const char *const ERROR_MSG = "Invalid heartbeat!";

static dots_task_env *curr_env = NULL;
static dots_task_env *org_env = NULL;
static struct coap_session_t *new_sess = NULL;
static struct coap_session_t *o_sess = NULL;

void dots_set_env(dots_task_env *env) {
    curr_env = env;
}

void dots_set_org_env(dots_task_env *env) {
    org_env = env;
}

void dots_set_new_sess(coap_session_t *sess) {
    new_sess = sess;
}

void dots_set_o_sess(coap_session_t *sess) {
    o_sess = sess;
}


static void restart_connection(dots_task_env *env) {
    log_info("Restart CoAP connection!");
    check_valid(connect_signal_channel(env), "connect_signal_channel() failed!");
}

static void handle_response(dots_task_env* env, coap_pdu_t* pdu) {

}

static void handle_request_timeout(dots_task_env* env, coap_pdu_t* pdu) {

}


void heartbeat_handler(
        coap_context_t *ctx,
        struct coap_resource_t *resource,
        coap_session_t *sess,
        coap_pdu_t *request,
        coap_binary_t *token,
        coap_string_t *query,
        coap_pdu_t *response) {
    log_info("Handle receive heartbeat from server.");

    size_t data_len;
    uint8_t *data;
    coap_get_data(request, &data_len, &data);

    // MessageId
    response->tid = request->tid;
    if (!validate_cbor_heartbeat_body(data, data_len)) {
        response->code = ResponseBadRequest;
        response->type = COAP_MESSAGE_NON; // TypeNon
        coap_add_data(response, strlen(ERROR_MSG), ERROR_MSG);
    } else {
        response->code = ResponseChanged;
        response->type = COAP_MESSAGE_NON;
    }

    // TODO: setIsReceivedHeartBeat
}



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
}

void response_handler(struct coap_context_t *context,
                      coap_session_t *session,
                      coap_pdu_t *sent,
                      coap_pdu_t *received,
                      const coap_tid_t id) {
    if (coap_get_log_level() < LOG_DEBUG) {
        coap_show_pdu(LOG_INFO, received);
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