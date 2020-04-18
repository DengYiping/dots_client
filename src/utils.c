//
// Created by Yiping Deng on 4/12/20.
//

#include "utils.h"

#include "log.h"
#include "preconditions.h"
#include <sys/socket.h>
#include <netdb.h>
#include <cbor.h>
#include <unistd.h>
#include <pthread.h>
#include "errno.h"

dots_task_env *connect_signal_channel(dots_task_env *env);

void restart_connection(dots_task_env *env) {
    log_info("Restart CoAP connection!");
    env->expecting_heartbeat = 0;
    env->reset_connection_flag = 0;
    map_deinit(&env->pending_request_map);
    map_init(&env->pending_request_map);
    check_valid(connect_signal_channel(env), "connect_signal_channel() failed!");
}

coap_address_t *
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

void dots_describe_pdu(coap_pdu_t *pdu) {
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

typedef struct delayed_run_args {
    long delay_time_milli;
    delayed_runnable_t runnable;
    void *task_args;
} delayed_run_args;

static int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

static void delayed_run_helper(delayed_run_args *args) {
    msleep(args->delay_time_milli);
    (args->runnable)(args->task_args);
    free(args);
}

void delayed_run(delayed_runnable_t runnable, long delay_time_milli, void *task_args) {
    delayed_run_args *args = malloc(sizeof(delayed_run_args));
    args->runnable = runnable;
    args->delay_time_milli = delay_time_milli;
    args->task_args = task_args;
    pthread_t delayed_thread;
    check_valid(
            !pthread_create(&delayed_thread, NULL, delayed_run_helper, args),
            "Cannot create a new thread!");
    pthread_detach(delayed_thread);
}