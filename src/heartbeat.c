//
// Created by Yiping Deng on 4/11/20.
//

#include "heartbeat.h"
#include <cbor.h>
#include <stdio.h>
#include "log.h"

#define CBOR_HEARTBEAKT_KEY 49
#define CBOR_PEER_HB_STATUS_KEY 51

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
 */
int validate_cbor_heartbeat_body(uint8_t *buffer, size_t len) {
    struct cbor_load_result result;
    cbor_item_t *item = cbor_load(buffer, len, &result);

    if (log_get_level() <= LOG_LEVEL_DEBUG) {
        cbor_describe(item, stdout);
    }

    int map_size = cbor_map_size(item);
    if (map_size != 1) {
        log_error("Heartbeat is invalid! Top level map size is %i, expecting 1!", map_size);
        return 0;
    }

    struct cbor_pair *heartbeat_pair = cbor_map_handle(item);
    int heartbeat_key = cbor_get_int(heartbeat_pair->key);
    if (heartbeat_key != CBOR_HEARTBEAKT_KEY) {
        log_error("Heartbeat has key of %i, expecting %i", heartbeat_key, CBOR_HEARTBEAKT_KEY);
        return 0;
    }

    map_size = cbor_map_size(heartbeat_pair->value);
    if (map_size != 0) {
        log_error("Heartbeat is invalid! Nested map inside of heartbeat has size %i, expecting 1!", map_size);
        return 0;
    }

    struct cbor_pair *status_pair = cbor_map_handle(heartbeat_pair->value);
    if (cbor_get_int(status_pair->key) != CBOR_PEER_HB_STATUS_KEY || cbor_get_bool(status_pair->value) != true) {
        log_error("Peer hb status doesn't exists or it is not true!");
        return 0;
    }

    cbor_decref(&item);
    return 1;
}
