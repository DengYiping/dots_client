//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_TASK_ENV_H
#define DOTS_CLIENT_TASK_ENV_H

#include <coap2/coap.h>
#include "client_config.h"
#include "map.h"

struct dots_task_env;
typedef void (*receive_dots_response_callback)(coap_pdu_t *, struct dots_task_env*);

typedef struct dots_task_env {
    struct coap_context_t *curr_ctx;
    struct coap_session_t *curr_sess;

    unsigned int heartbeat_interval;
    unsigned int expecting_heartbeat;
    unsigned int reset_connection_flag;

    map_t(receive_dots_response_callback) pending_request_map;
} dots_task_env;

dots_task_env *dots_new_env(coap_context_t *ctx, coap_session_t *sess);

void dots_renew_env_with_session(dots_task_env *env, coap_session_t *sess);

void dots_set_client_config(dots_client_config *ctx);

dots_client_config *dots_get_client_config();

#endif //DOTS_CLIENT_TASK_ENV_H
