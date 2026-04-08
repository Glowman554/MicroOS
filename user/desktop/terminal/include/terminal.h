#pragma once

#include <stdbool.h>

#define TERM_OUTPUT_SIZE  (64 * 1024)
#define TERM_INPUT_SIZE   2048
#define TERM_HISTORY_MAX  64
#define PIPE_BUFFER_SIZE  65536

typedef struct {
    char* output_buf;
    int   output_len;

    char  input_buf[TERM_INPUT_SIZE + 1];
    int   input_len;

    char** history;
    int    history_size;
    int    history_index;

    int    scroll_offset;

    char** envp;
} terminal_state_t;
