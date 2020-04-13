//
// Created by Yiping Deng on 4/11/20.
//

#include "heartbeat.h"
#include <cbor.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"
#include "task_env.h"
#include "preconditions.h"
#include "utils.h"

#define CBOR_HEARTBEAKT_KEY 49
#define CBOR_PEER_HB_STATUS_KEY 51
#define HEARTBEAT_RECONNECT_LIMIT 5

static const char *const HB_REQUEST_PATH_WELL_KNOWN = ".well-known";
static const char *const HB_REQUEST_PATH_DOTS = "dots";
static const char *const HB_REQUEST_PATH_HB = "hb";

static pthread_t heartbeat_thread = NULL;

/**
 *      Header: PUT (Code=0.03)
        Uri-Path: ".well-known"
        Uri-Path: "dots"
        Uri-Path: "hb"
        Content-Format: "application/dots+cbor"

        {
          "ietf-dots-signal-channel:heartbeat": {
             "peer-hb-status": true
           }
        }

   The DOTS Heartbeat mechanism uses non-confirmable PUT requests
   (Figure 27) with an expected 2.04 (Changed) Response Code
   (Figure 28).  This procedure occurs between a DOTS agent and its
   immediate peer DOTS agent.  As such, this PUT request MUST NOT be
   relayed by a DOTS gateway.  The PUT request used for DOTS heartbeat
   MUST NOT have a 'cuid', 'cdid,' or 'mid' Uri-Path.
 */
static void create_cbor_heartbeat(uint8_t **buffer_ptr, size_t *len_ptr) {
    cbor_item_t *nested = cbor_new_definite_map(1);
    cbor_map_add(nested, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(CBOR_PEER_HB_STATUS_KEY)),
            .value = cbor_move(cbor_build_bool(true))
    });
    cbor_item_t *root = cbor_new_definite_map(1);
    cbor_map_add(root, (struct cbor_pair) {
            .key = cbor_move(cbor_build_uint8(CBOR_HEARTBEAKT_KEY)),
            .value = cbor_move(nested)
    });
    size_t new_len = cbor_serialize_alloc(root, buffer_ptr, len_ptr);
    if (new_len != *len_ptr) {
        *len_ptr = new_len;
    }
    cbor_decref(&root);
}

// Tested
int validate_cbor_heartbeat_body(uint8_t *buffer, size_t len) {
    struct cbor_load_result result;
    cbor_item_t *item = cbor_load(buffer, len, &result);

    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        cbor_describe(item, stdout);
    }

    int map_size = cbor_map_size(item);
    if (map_size != 1) {
        log_error("Heartbeat is invalid! Top level map size is %i, expecting 1!", map_size);
        cbor_decref(&item);
        return 0;
    }

    struct cbor_pair *heartbeat_pair = cbor_map_handle(item);
    int heartbeat_key = cbor_get_int(heartbeat_pair->key);
    if (heartbeat_key != CBOR_HEARTBEAKT_KEY) {
        log_error("Heartbeat has key of %i, expecting %i", heartbeat_key, CBOR_HEARTBEAKT_KEY);
        cbor_decref(&item);
        return 0;
    }

    map_size = cbor_map_size(heartbeat_pair->value);
    if (map_size != 1) {
        log_error("Heartbeat is invalid! Nested map inside of heartbeat has size %i, expecting 1!", map_size);
        cbor_decref(&item);
        return 0;
    }

    struct cbor_pair *status_pair = cbor_map_handle(heartbeat_pair->value);
    if (cbor_get_int(status_pair->key) != CBOR_PEER_HB_STATUS_KEY || cbor_get_bool(status_pair->value) != true) {
        log_error("Peer hb status doesn't exists or it is not true!");
        cbor_decref(&item);
        return 0;
    }

    cbor_decref(&item);
    return 1;
}

static void heartbeat_send(dots_task_env *env) {
    if (!env->curr_sess) {
        log_info("Connection hasn't been established!");
        restart_connection(env);
        return;
    }

    env->expecting_heartbeat = env->expecting_heartbeat + 1;
    if (env->expecting_heartbeat > HEARTBEAT_RECONNECT_LIMIT) {
        coap_session_release(env->curr_sess);
        return;
    }

    log_debug("Sending a heartbeat using session %p!", env->curr_sess);
    uint8_t *buffer;
    size_t buffer_len;
    create_cbor_heartbeat(&buffer, &buffer_len);
    check_valid(buffer, "Unable to deserialize heartbeat payload");
    uint16_t message_id = coap_new_message_id(env->curr_sess);
    unsigned char buf[3];

    coap_pdu_t *pdu = coap_pdu_init(
            COAP_MESSAGE_NON,
            COAP_REQUEST_PUT,
            message_id,
            coap_session_max_pdu_size(env->curr_sess));
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(HB_REQUEST_PATH_WELL_KNOWN), HB_REQUEST_PATH_WELL_KNOWN);
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(HB_REQUEST_PATH_DOTS), HB_REQUEST_PATH_DOTS);
    coap_add_option(pdu, COAP_OPTION_URI_PATH, strlen(HB_REQUEST_PATH_HB), HB_REQUEST_PATH_HB);
    coap_add_option(pdu, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_CBOR), buf);

    coap_add_data(pdu, buffer_len, buffer);

    dots_describe_pdu(pdu);

    char map_key[64];
    memset(map_key, 0, sizeof(map_key));
    sprintf(map_key, "%d", pdu->tid);

    coap_send(env->curr_sess, pdu);
    map_set(&env->pending_heartbeat_map, map_key, 1);

    free(buffer);
}

static void active_heartbeat_runnable(dots_task_env *env) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    while (1) {
        sleep(env->heartbeat_interval);
        heartbeat_send(env);
    }
}

void start_heartbeat(dots_task_env *env) {
    if (!heartbeat_thread) {
        log_info("Create heartbeat ticker! It will tick every %i seconds!", env->heartbeat_interval);
        check_valid(
                !pthread_create(&heartbeat_thread, NULL, active_heartbeat_runnable, env),
                "Cannot create a new thread!");
        pthread_detach(heartbeat_thread);
    }
}

void stop_heartbeat() {
    if (heartbeat_thread) {
        pthread_cancel(heartbeat_thread);
    }
}


