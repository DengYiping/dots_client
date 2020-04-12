//
// Created by Yiping Deng on 4/12/20.
//

#ifndef DOTS_CLIENT_UTILS_H
#define DOTS_CLIENT_UTILS_H

#include <coap2/coap.h>

coap_address_t* resolve_address(const char *host, const char *service);

#endif //DOTS_CLIENT_UTILS_H
