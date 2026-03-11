#include "output.h"

#include <string.h>
#include <stdio.h>

#include <sys/env.h>

#define GET_CWD(cwd) char cwd[64] = { 0 }; set_env(SYS_GET_PWD_ID, cwd)

void term_append(terminal_state_t* st, const char* text, int len) {
    if (len <= 0) {
        return;
    }

    int space = TERM_OUTPUT_SIZE - st->output_len - 1;
    if (len > space) {
        int drop = TERM_OUTPUT_SIZE / 2;
        memmove(st->output_buf, st->output_buf + drop, st->output_len - drop);
        st->output_len -= drop;
        space = TERM_OUTPUT_SIZE - st->output_len - 1;
        
        if (len > space) {
            len = space;
        }
    }
    memcpy(st->output_buf + st->output_len, text, len);
    st->output_len += len;
    st->output_buf[st->output_len] = '\0';
}

void term_puts(terminal_state_t* st, const char* str) {
    term_append(st, str, strlen(str));
}

void term_printf(terminal_state_t* st, const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int n = vsprintf(buf, fmt, args);
    va_end(args);
    term_append(st, buf, n);
}

void term_print_prompt(terminal_state_t* st) {
    GET_CWD(cwd);
    term_printf(st, "\nshell %s > ", cwd);
}
