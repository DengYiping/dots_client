//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H
#define DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H

#include <coap2/coap.h>
#include "task_env.h"

void heartbeat_handler(coap_context_t *,
                       struct coap_resource_t *,
                       coap_session_t *,
                       coap_pdu_t *,
                       coap_binary_t * /* token */,
                       coap_string_t * /* query string */,
                       coap_pdu_t * /* response */);

void event_handler(struct coap_context_t *,
                   coap_event_t event,
                   struct coap_session_t *session);

void response_handler(struct coap_context_t *context,
                      coap_session_t *session,
                      coap_pdu_t *sent,
                      coap_pdu_t *received,
                      const coap_tid_t id);

void dots_set_env(dots_task_env *env);

void dots_set_org_env(dots_task_env *env);

void dots_set_o_sess(coap_session_t *sess);

#endif //DOTS_CLIENT_SIGNAL_CHANNEL_HANDLERS_H
