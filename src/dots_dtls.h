//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_DOTS_DTLS_H
#define DOTS_CLIENT_DOTS_DTLS_H

#include <coap2/coap.h>

enum {DOTS_PEER_CLIENT, DOTS_PEER_SERVER};
typedef struct dtls_params {
    char* ca_filename;
    char* ca_path;
    char* certificate_filename;
    char* private_key_filename;

    // Pinned Certificate
    char* reference_identifier;
    char* present_identifier_list;
} dtls_params;

/**
 *
 * @param addr Nullable. The address of current machine
 * @param params Parameters for dtls
 * @param ctx_peer
 * @return New context created
 */
struct coap_context_t* new_context_dtls(coap_address_t* addr, dtls_params* params, int ctx_peer);

#endif //DOTS_CLIENT_DOTS_DTLS_H
