//
// Created by Yiping Deng on 4/18/20.
//

#ifndef DOTS_CLIENT_REQUESTER_H
#define DOTS_CLIENT_REQUESTER_H

#include <cbor.h>
#include <coap2/coap.h>
#include "task_env.h"
#include "link_list.h"

enum {
    HB_REQUEST, MITIGATION_REQUEST, CONFIG_REQUEST
};

typedef void (*send_dots_request_callback)(coap_pdu_t *, dots_task_env *);

void send_dots_request(
        int request_type,
        cbor_item_t *payload,
        dots_task_env* env,
        list_node* extra_uris,
        send_dots_request_callback callback,
        receive_dots_response_callback response_callback);

#endif //DOTS_CLIENT_REQUESTER_H
