//
// Created by Yiping Deng on 4/18/20.
//

#include "mitigation.h"
#include "requester.h"
#include <cbor.h>
#include "utils.h"
#include "log.h"

#define MITIGATION_SCOPE_KEY 1
#define SCOPE_KEY 2
#define TARGET_PREFIX_KEY 6
#define TARGET_PORT_RANGE_KEY 7
#define LOWER_PORT_KEY 8
#define UPPER_PORT_KEY 9
#define TARGET_PROTOCOL_KEY 10
#define TARGET_FQDN_KEY 11
#define TARGET_URI_KEY 12
#define ALIAS_NAME_KEY 13
#define LIFETIME_KEY 14
#define TRIGGER_MITIGATION_KEY 45

static cbor_item_t *generate_mitigation_request_body() {
    cbor_item_t *root = cbor_new_definite_map(1);
    cbor_item_t *mitigation_scope_value = cbor_new_definite_map(1);
    cbor_item_t *scope_value = cbor_new_definite_array(1);
    cbor_item_t *scope_item = cbor_new_definite_map(2);
    cbor_item_t *prefix_list = cbor_new_definite_array(1);

    cbor_array_set(prefix_list, 0, cbor_build_string("10.100.0.0/24"));
    cbor_map_add(scope_item, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(LIFETIME_KEY)),
            .value = cbor_move(cbor_build_uint32(3600))
    });
    cbor_map_add(scope_item, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(TARGET_PREFIX_KEY)),
            .value = cbor_move(prefix_list)
    });
    cbor_array_set(scope_value, 0, scope_item);
    cbor_map_add(mitigation_scope_value, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(SCOPE_KEY)),
            .value = cbor_move(scope_value)
    });
    cbor_map_add(root, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(MITIGATION_SCOPE_KEY)),
            .value = cbor_move(mitigation_scope_value)
    });

    return root;
}

static void request_response_callback(coap_pdu_t *pdu, dots_task_env *env) {
    if (pdu->code == COAP_RESPONSE_200) {
        log_info("Successfully handled the mitigation requests!");
    }
    dots_describe_pdu(pdu);
}

static void request_send_callback(coap_pdu_t* pdu, dots_task_env* env) {
    dots_describe_pdu(pdu);
}

void send_mitigation_request(dots_task_env *env) {
    list_node *uris = list_add(NULL, "cuid=dz6pHjaADkaFTbjr0JGBpw");
    uris = list_add(uris, "mid=123");
    send_dots_request(
            MITIGATION_REQUEST,
            generate_mitigation_request_body(),
            env,
            uris,
            request_send_callback,
            request_response_callback);
}