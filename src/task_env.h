//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_TASK_ENV_H
#define DOTS_CLIENT_TASK_ENV_H
#include <coap2/coap.h>
#include "client_context.h"

typedef struct dots_task_env {
    struct coap_context_t* curr_ctx;
    struct coap_session_t* curr_sess;
    struct coap_session_t* replacing_sess;
} dots_task_env;

dots_task_env* dots_new_env(struct coap_context_t* ctx, struct coap_session_t* sess);
void dots_set_client_context(dots_client_context* ctx);

dots_client_context* dots_get_client_context();
#endif //DOTS_CLIENT_TASK_ENV_H
