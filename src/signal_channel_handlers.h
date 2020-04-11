//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H
#define DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H
#include <coap2/coap.h>

void heartbeat_handler(coap_context_t *,
                       struct coap_resource_t *,
                       coap_session_t *,
                       coap_pdu_t *,
                       coap_binary_t * /* token */,
                       coap_string_t * /* query string */,
                       coap_pdu_t * /* response */);

#endif //DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H
