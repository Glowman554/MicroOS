#include <gdb/gdb.h>

#include <stddef.h>
#include <stdbool.h>

int gdb_get_val(char digit, int base) {
    int value;

    if ((digit >= '0') && (digit <= '9')) {
        value = digit - '0';
    } else if ((digit >= 'a') && (digit <= 'f')) {
        value = digit-'a' + 0xa;
    } else if ((digit >= 'A') && (digit <= 'F')) {
        value = digit-'A' + 0xa;
    } else {
        return GDB_EOF;
    }

    return (value < base) ? value : GDB_EOF;
}

int gdb_strtol(const char* str, unsigned int len, int base, const char** endptr) {
    if (endptr) {
        *endptr = NULL;
    }

    if (len < 1) {
        return 0;
    }

    unsigned int pos = 0;
    int sign = 1;
    if (str[pos] == '-') {
        sign = -1;
        pos += 1;
    } else if (str[pos] == '+') {
        sign = 1;
        pos += 1;
    }

    if ((pos + 2 < len) && (str[pos] == '0') &&
        ((str[pos + 1] == 'x') || (str[pos + 1] == 'X'))) {
        base = 16;
        pos += 2;
    }

    if (base == 0) {
        base = 10;
    }

    int value = 0;
    bool valid = false;
    for (; (pos < len) && (str[pos] != '\x00'); pos++) {
        int tmp = gdb_get_val(str[pos], base);
        if (tmp == GDB_EOF) {
            break;
        }

        value = value * base + tmp;
        valid = true;
    }

    if (!valid) {
        return 0;
    }

    if (endptr) {
        *endptr = str+pos;
    }

    value *= sign;

    return value;
}