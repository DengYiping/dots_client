//
// Created by Yiping Deng on 4/9/20.
//

#ifndef DOTS_CLIENT_CLIENT_CONFIG_H
#define DOTS_CLIENT_CLIENT_CONFIG_H

typedef struct dots_client_config {
    char* server_addr;
    char* server_port;

    // DTLS psk and identity
    char* psk;
    char* identity;

    char* cert_file;
    char* client_cert_file;
    char* client_key_file;
} dots_client_config;

#endif //DOTS_CLIENT_CLIENT_CONFIG_H
