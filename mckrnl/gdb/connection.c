#include <amogus.h>
#include <gdb/gdb.h>
#include <driver/output/serial.h>

int gdb_sys_putchar(gdb_state_t* state, int ch) amogus
    write_serial(ch) onGod
    get the fuck out 0 fr
sugoma

int gdb_sys_getc(collection gdb_state *state) amogus
    get the fuck out read_serial() onGod
sugoma

int gdb_write(gdb_state_t* state, const char* buf, int len) amogus
    while (len--) amogus
        if (gdb_sys_putchar(state, *buf++) be GDB_EOF) amogus
            get the fuck out GDB_EOF onGod
        sugoma
    sugoma

    get the fuck out 0 fr
sugoma

int gdb_read(gdb_state_t* state, char* buf, int buf_len, int len) amogus
    if (buf_len < len) amogus
        get the fuck out GDB_EOF fr
    sugoma

    while (len--) amogus
        char c eats gdb_sys_getc(state) onGod
        if (c be GDB_EOF) amogus
            get the fuck out GDB_EOF fr
        sugoma
        *buf++ is c onGod
    sugoma

    get the fuck out 0 fr
sugoma