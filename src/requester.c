//
// Created by Yiping Deng on 4/18/20.
//

#include "requester.h"
#include "preconditions.h"

static const char *const REQUEST_PATH_WELL_KNOWN = ".well-known";
static const char *const REQUEST_PATH_DOTS = "dots";

static const char *REQUEST_TYPE_TO_REQUEST_SUFFIX[] = {
        [HB_REQUEST] = "hb",
        [MITIGATION_REQUEST] = "mitigation",
        [CONFIG_REQUEST] = "config"
};

void send_dots_request(
        int request_type,
        cbor_item_t *payload,
        dots_task_env* env,
        send_dots_request_callback callback,
        receive_dots_response_callback response_callback) {
    coap_session_t* session = env->curr_sess;

    uint8_t *buffer_ptr;
    size_t buffer_len, payload_len;

    payload_len = cbor_serialize_alloc(payload, &buffer_ptr, &buffer_len);
    check_valid(payload_len > 0, "Cannot serialize cbor payload");
    cbor_decref(&payload);

    uint16_t message_id = coap_new_message_id(session);

    coap_pdu_t *pdu = coap_pdu_init(
            COAP_MESSAGE_NON,
            COAP_REQUEST_PUT,
            message_id,
            coap_session_max_pdu_size(session));
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(REQUEST_PATH_WELL_KNOWN), REQUEST_PATH_WELL_KNOWN);
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(REQUEST_PATH_DOTS), REQUEST_PATH_DOTS);
    coap_add_option(
            pdu,
            COAP_OPTION_URI_PATH,
            strlen(REQUEST_TYPE_TO_REQUEST_SUFFIX[request_type]),
            REQUEST_TYPE_TO_REQUEST_SUFFIX[request_type]);
    unsigned char buf[3];
    coap_add_option(pdu, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_CBOR), buf);
    coap_add_data(pdu, payload_len, buffer_ptr);

    if (callback != NULL) {
        callback(pdu, env);
    }

    char map_key[64];
    memset(map_key, 0, sizeof(map_key));
    sprintf(map_key, "%d", pdu->tid);
    map_set(&env->pending_request_map, map_key, response_callback);

    coap_send(session, pdu);
    free(buffer_ptr);
}



