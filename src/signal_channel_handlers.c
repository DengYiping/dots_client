//
// Created by Yiping Deng on 4/11/20.
//

#include "signal_channel_handlers.h"
#include "log.h"
#include "heartbeat.h"
#include "dots_code.h"

static const char* const ERROR_MSG = "Invalid heartbeat!";

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
    uint8_t* data;
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
}