#pragma once

#include <stdint.h>

typedef struct coro_t {
    int line;
} coro_t;

#define coro_begin(coro) switch (coro->line) { case 0: coro->line = 0;
#define coro_yield(coro) { coro->line = __LINE__; return; case __LINE__:;}
#define coro_cond_yield(coro, condition) while (!(condition)) { coro_yield(coro); }
#define coro_exit(coro) { coro->line = __LINE__; } }
#define coro_reset(coro) { coro->line = 0; } }