#pragma once

#include <stdbool.h>
#include <non-standard/net/nettools.h>

typedef struct {
    char domain[64];
} resolve_request_t;

typedef struct {
    char domain[64];
    ip_u ip;
} resolve_reply_t;

ip_u resolved_A(char* domain);