//
// Created by Yiping Deng on 4/12/20.
//

#include "messages.h"

char* pdu_as_map_key(coap_pdu_t* pdu) {
    char* buffer = malloc(pdu->token_length);
    memcpy(buffer, pdu->token, pdu->token_length);
    return buffer;
}
