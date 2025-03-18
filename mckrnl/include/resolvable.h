#pragma once

#include <stdint.h>

#define STATE_INIT 0
#define STATE_REQUEST 1
#define STATE_WAIT 2
#define STATE_DONE 255

typedef struct resolvable {
    uint8_t state;
    uint8_t buffer[128];
} resolvable_t;

#define is_resolved(resolvable) ((resolvable)->state == STATE_DONE)
#define cast_buffer(resolvable, type) ((type*) (resolvable)->buffer)
