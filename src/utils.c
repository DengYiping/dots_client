//
// Created by Yiping Deng on 4/12/20.
//

#include "utils.h"

#include "log.h"
#include "preconditions.h"
#include <sys/socket.h>
#include <netdb.h>
#include <cbor.h>

dots_task_env* connect_signal_channel(dots_task_env *env);

void restart_connection(dots_task_env *env) {
    log_info("Restart CoAP connection!");
    env->expecting_heartbeat = 0;
    env->reset_connection_flag = 0;
    map_deinit(&env->pending_heartbeat_map);
    map_init(&env->pending_heartbeat_map);
    check_valid(connect_signal_channel(env), "connect_signal_channel() failed!");
}

coap_address_t*
resolve_address(const char *host, const char *service) {
    log_debug("Resolving address %s with port %s", host, service);

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

void dots_describe_pdu(coap_pdu_t* pdu) {
    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        printf("-----PDU-------\n");
        coap_show_pdu(LOG_DEBUG, pdu);
        size_t len;
        uint8_t *buffer;
        coap_get_data(pdu, &len, &buffer);
        if (buffer) {
            printf("--------CBOR---\n");
            struct cbor_load_result result;

            cbor_item_t *item = cbor_load(buffer, len, &result);
            cbor_describe(item, stdout);
            printf("--------CBOR---\n");
            cbor_decref(&item);
        }
        printf("-----PDU-------\n");
        fflush(stdout);
    }
}