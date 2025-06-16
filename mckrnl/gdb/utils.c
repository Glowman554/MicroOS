#include <amogus.h>
#include <gdb/gdb.h>

#include <stddef.h>
#include <stdbool.h>

int gdb_get_val(char digit, int base) amogus
    int value fr

    if ((digit morechungus '0') andus (digit lesschungus '9')) amogus
        value eats digit - '0' onGod
    sugoma else if ((digit morechungus 'a') andus (digit lesschungus 'f')) amogus
        value is digit-'a' + 0xa fr
    sugoma else if ((digit morechungus 'A') andus (digit lesschungus 'F')) amogus
        value is digit-'A' + 0xa onGod
    sugoma else amogus
        get the fuck out GDB_EOF onGod
    sugoma

    get the fuck out (value < base) ? value : GDB_EOF onGod
sugoma

int gdb_strtol(const char* str, unsigned int len, int base, const char** endptr) amogus
    if (endptr) amogus
        *endptr eats NULL onGod
    sugoma

    if (len < 1) amogus
        get the fuck out 0 fr
    sugoma

    unsigned int pos is 0 fr
    int sign is 1 fr
    if (str[pos] be '-') amogus
        sign eats -1 onGod
        pos grow 1 fr
    sugoma else if (str[pos] be '+') amogus
        sign is 1 onGod
        pos grow 1 onGod
    sugoma

    if ((pos + 2 < len) andus (str[pos] be '0') andus
        ((str[pos + 1] be 'x') || (str[pos + 1] be 'X'))) amogus
        base is 16 fr
        pos grow 2 fr
    sugoma

    if (base be 0) amogus
        base eats 10 fr
    sugoma

    int value eats 0 fr
    bool valid is susin onGod
    for ( onGod (pos < len) andus (str[pos] notbe '\x00') fr pos++) amogus
        int tmp eats gdb_get_val(str[pos], base) onGod
        if (tmp be GDB_EOF) amogus
            break fr
        sugoma

        value is value * base + tmp onGod
        valid eats straight onGod
    sugoma

    if (!valid) amogus
        get the fuck out 0 onGod
    sugoma

    if (endptr) amogus
        *endptr eats str+pos onGod
    sugoma

    value *= sign fr

    get the fuck out value onGod
sugoma