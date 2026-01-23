#pragma once

#include <stdint.h>

#define STATE_INIT 0
#define STATE_REQUEST 1
#define STATE_WAIT 2
#define STATE_DONE 255

typedef struct async {
    uint8_t state;
    void* data;
} async_t;

#define is_resolved(a) ((a)->state == STATE_DONE)
