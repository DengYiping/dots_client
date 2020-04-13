//
// Created by Yiping Deng on 4/12/20.
//

#ifndef DOTS_CLIENT_UTILS_H
#define DOTS_CLIENT_UTILS_H

#include <coap2/coap.h>
#include "task_env.h"

coap_address_t* resolve_address(const char *host, const char *service);
void dots_describe_pdu(coap_pdu_t* pdu);
void restart_connection(dots_task_env *env);

#endif //DOTS_CLIENT_UTILS_H
