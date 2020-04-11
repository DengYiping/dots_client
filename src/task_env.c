//
// Created by Yiping Deng on 4/11/20.
//

#include "task_env.h"


static dots_client_context* curr_client_context;

dots_task_env* dots_new_env(struct coap_context_t* ctx, struct coap_session_t* sess) {
    dots_task_env* result = malloc(sizeof(dots_task_env));
    result->curr_ctx = ctx;
    result->curr_sess = sess;
    result->replacing_sess = NULL;
    return result;
}

void dots_set_client_context(dots_client_context* ctx) {
    curr_client_context = ctx;
}

dots_client_context* dots_get_client_context() {
    return curr_client_context;
}
