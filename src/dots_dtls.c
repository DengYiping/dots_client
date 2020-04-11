//
// Created by Yiping Deng on 4/11/20.
//

#include "dots_dtls.h"
#include "preconditions.h"
#include <openssl/x509.h>
#include <openssl/pem.h>

typedef struct coap_strlist_t {
    struct coap_strlist_t *next;
    coap_string_t* str;
} coap_strlist_t;


/**
 *
 * @param addr Nullable. The address of current machine. Listening address.
 * @param params Parameters for dtls
 * @param ctx_peer
 * @return
 */
struct coap_context_t* new_context_dtls(coap_address_t* addr, dtls_params* params, int ctx_peer) {
    struct coap_context_t* ctx = coap_new_context(addr);

    coap_dtls_pki_t* setup_data = malloc(sizeof(coap_dtls_pki_t));

    // Setup dtls pki configuration
    setup_data->version = COAP_DTLS_PKI_SETUP_VERSION;
    setup_data->pki_key.key_type = COAP_PKI_KEY_PEM;
    setup_data->verify_peer_cert = 1;
    setup_data->require_peer_cert = 1;
    setup_data->allow_self_signed = 1;
    setup_data->allow_expired_certs = 1;
    setup_data->cert_chain_validation = 1;
    setup_data->cert_chain_verify_depth = 2;

    // Use for check that is certificate in certificate revocation list (CRL) from actual server.
    setup_data->check_cert_revocation = 1;
    setup_data->allow_no_crl = 1;
    setup_data->allow_expired_crl = 1;

    switch (ctx_peer) {
        case DOTS_PEER_CLIENT:
            break;
        case DOTS_PEER_SERVER:
            setup_data->validate_cn_call_back = NULL;
            setup_data->cn_call_back_arg = NULL;
            break;
        default:
            panic("Peer type is not supported!");
    }

    return ctx;
}

coap_strlist_t* get_domain_name_from_certificate_file(char* certificate_fname) {
    coap_strlist_t* head;
    coap_strlist_t* tail;

    FILE* fp = fopen(certificate_fname, "r");
    check_valid(fp, "unable to open: %s", certificate_fname);

    X509* cert = PEM_read_X509(fp, NULL, NULL, NULL);
    check_valid(cert, "unable to parse certificate in: %s", certificate_fname);

    X509_NAME* subj = X509_get_subject_name(cert);
    for (int i = 0; i < X509_NAME_entry_count(subj); i++) {
        X509_NAME_ENTRY *e = X509_NAME_get_entry(subj, i);
        ASN1_STRING *d = X509_NAME_ENTRY_get_data(e);
        char *str = ASN1_STRING_data(d);
    }

    X509_free(cert);
    fclose(fp);
    return NULL;
}


