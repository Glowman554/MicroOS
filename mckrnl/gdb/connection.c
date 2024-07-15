#include <gdb/gdb.h>
#include <driver/output/serial.h>

int gdb_sys_putchar(gdb_state_t* state, int ch) {
    write_serial(ch);
    return 0;
}

int gdb_sys_getc(struct gdb_state *state) {
    return read_serial();
}

int gdb_write(gdb_state_t* state, const char* buf, int len) {
    while (len--) {
        if (gdb_sys_putchar(state, *buf++) == GDB_EOF) {
            return GDB_EOF;
        }
    }

    return 0;
}

int gdb_read(gdb_state_t* state, char* buf, int buf_len, int len) {
    if (buf_len < len) {
        return GDB_EOF;
    }

    while (len--) {
        char c = gdb_sys_getc(state);
        if (c == GDB_EOF) {
            return GDB_EOF;
        }
        *buf++ = c;
    }

    return 0;
}