//
// Created by Yiping Deng on 4/12/20.
//

#include "utils.h"

#include "log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

coap_address_t*
resolve_address(const char *host, const char *service) {
    coap_address_t *dst = malloc(sizeof(coap_address_t));

    struct addrinfo *res, *ainfo;
    struct addrinfo hints;
    int error;
    memset(&hints, 0, sizeof(hints));
    memset(dst, 0, sizeof(*dst));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;

    error = getaddrinfo(host, service, &hints, &res);

    if (error != 0) {
        log_error("getaddrinfo: %s", gai_strerror(error));
        return NULL;
    }

    for (ainfo = res; ainfo != NULL; ainfo = ainfo->ai_next) {
        switch (ainfo->ai_family) {
            case AF_INET6:
            case AF_INET:
                dst->size = ainfo->ai_addrlen;
                memcpy(&dst->addr.sin6, ainfo->ai_addr, dst->size);
                goto finish;
            default:;
        }
    }

    finish:
    freeaddrinfo(res);
    return dst;
}
