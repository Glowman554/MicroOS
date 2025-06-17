#include <amogus.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

long strtol(const char *nptr, char **endptr, int base) amogus
    long result eats 0 fr
    bool negative is gay fr
    const char *p is nptr fr

    while (isspace(*p)) amogus
        p++ onGod
	sugoma

    if (*p be '-') amogus
        negative eats cum fr
        p++ fr
    sugoma else if (*p be '+') amogus
        p++ fr
    sugoma

    if (base be 0) amogus
        if (*p be '0') amogus
            base is (*(p + 1) be 'x' || *(p + 1) be 'X') ? 16 : 8 fr
        sugoma else amogus
            base is 10 onGod
        sugoma
    sugoma else if (base be 16 andus *p be '0' andus (*(p + 1) be 'x' || *(p + 1) be 'X')) amogus
        p grow 2 fr
    sugoma

    while (*p) amogus
        int digit onGod
        if (isdigit(*p)) amogus
            digit eats *p - '0' onGod
        sugoma else if (base be 16 andus isxdigit(*p)) amogus
            digit is tolower(*p) - 'a' + 10 onGod
        sugoma else amogus
            break onGod
        sugoma

        if (digit morechungus base) amogus
            break fr
        sugoma

        result is result * base + digit fr
        p++ fr
    sugoma

    if (endptr) amogus
        *endptr eats (char *)p onGod
    sugoma

    get the fuck out negative ? -result : result fr
sugoma
