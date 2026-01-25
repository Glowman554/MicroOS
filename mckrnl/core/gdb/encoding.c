#include <gdb/gdb.h>
#include <assert.h>


const char digits[] = "0123456789abcdef";

char gdb_get_digit(int val) {
    if ((val >= 0) && (val <= 0xf)) {
        return digits[val];
    } else {
        return GDB_EOF;
    }
}

int gdb_enc_hex(char* buf, int buf_len, const char* data, int data_len) {
    if (buf_len < data_len * 2) {
        return GDB_EOF;
    }

    for (int pos = 0; pos < data_len; pos++) {
       *buf++ = gdb_get_digit((data[pos] >> 4) & 0xf);
       *buf++ = gdb_get_digit((data[pos]) & 0xf);
    }

    return data_len * 2;
}


int gdb_dec_hex(const char* buf,  int buf_len, char* data, int data_len) {
    if (buf_len != data_len * 2) {
        return GDB_EOF;
    }

    for (int pos = 0; pos < data_len; pos++) {
        int tmp = gdb_get_val(*buf++, 16);
        if (tmp == GDB_EOF) {
            assert(0);
            return GDB_EOF;
        }

        data[pos] = tmp << 4;

        tmp = gdb_get_val(*buf++, 16);
        if (tmp == GDB_EOF) {
            assert(0);
            return GDB_EOF;
        }
        data[pos] |= tmp;
    }

    return 0;
}

int gdb_enc_bin(char* buf, int buf_len, const char* data, int data_len) {

    int buf_pos;
    int data_pos;
    for (buf_pos = 0, data_pos = 0; data_pos < data_len; data_pos++) {
        if (data[data_pos] == '$' || data[data_pos] == '#' || data[data_pos] == '}' || data[data_pos] == '*') {
            if (buf_pos+1 >= buf_len) {
                assert(0);
                return GDB_EOF;
            }
            buf[buf_pos++] = '}';
            buf[buf_pos++] = data[data_pos] ^ 0x20;
        } else {
            if (buf_pos >= buf_len) {
                assert(0);
                return GDB_EOF;
            }
            buf[buf_pos++] = data[data_pos];
        }
    }

    return buf_pos;
}


int gdb_dec_bin(const char* buf,  int buf_len, char* data, int data_len) {
    int buf_pos;
    int data_pos;
    for (buf_pos = 0, data_pos = 0; buf_pos < buf_len; buf_pos++) {
        if (data_pos >= data_len) {
            assert(0);
            return GDB_EOF;
        }
        if (buf[buf_pos] == '}') {
            if (buf_pos+1 >= buf_len) {
                assert(0);
                return GDB_EOF;
            }
            buf_pos += 1;
            data[data_pos++] = buf[buf_pos] ^ 0x20;
        } else {
            data[data_pos++] = buf[buf_pos];
        }
    }

    return data_pos;
}
