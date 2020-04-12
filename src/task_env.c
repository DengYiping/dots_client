//
// Created by Yiping Deng on 4/11/20.
//

#include "task_env.h"

static dots_client_config* client_config;

dots_task_env* dots_new_env(struct coap_context_t* ctx, struct coap_session_t* sess) {
    dots_task_env* result = malloc(sizeof(dots_task_env));
    result->curr_ctx = ctx;
    result->curr_sess = sess;
    result->replacing_sess = NULL;
    result->heartbeat_interval = 5; // Every 5 second by default
    return result;
}

void dots_set_client_config(dots_client_config* ctx) {
    client_config = ctx;
}

dots_client_config* dots_get_client_config() {
    return client_config;
}
