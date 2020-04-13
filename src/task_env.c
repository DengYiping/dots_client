//
// Created by Yiping Deng on 4/11/20.
//

#include "task_env.h"
#include "preconditions.h"

static dots_client_config* client_config;

dots_task_env* dots_new_env(coap_context_t* ctx, coap_session_t* sess) {
    dots_task_env* result = malloc(sizeof(dots_task_env));
    result->curr_ctx = ctx;
    result->curr_sess = sess;
    result->heartbeat_interval = 1; // Every 5 second by default
    result->expecting_heartbeat = 0;

    map_init(&result->pending_heartbeat_map);
    return result;
}

void dots_renew_env_with_session(dots_task_env* env, coap_session_t* sess) {
    log_info("Renewing a old session and disable the new session!");
    check_valid(env->curr_sess != sess, "Current session is the same as the renew session");
    if (env->curr_sess) {
        coap_session_release(env->curr_sess);
    }
    env->curr_sess = sess;
}
void dots_set_client_config(dots_client_config* ctx) {
    client_config = ctx;
}

dots_client_config* dots_get_client_config() {
    return client_config;
}
