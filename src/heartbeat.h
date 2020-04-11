//
// Created by Yiping Deng on 4/11/20.
//

#ifndef DOTS_CLIENT_HEARTBEAT_H
#define DOTS_CLIENT_HEARTBEAT_H

#include <stdlib.h>

int validate_cbor_heartbeat_body(uint8_t *buffer, size_t len);

#endif //DOTS_CLIENT_HEARTBEAT_H
