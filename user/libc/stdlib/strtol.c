#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

long strtol(const char *nptr, char **endptr, int base) {
    long result = 0;
    bool negative = false;
    const char *p = nptr;

    while (isspace(*p)) {
        p++;
	}

    if (*p == '-') {
        negative = true;
        p++;
    } else if (*p == '+') {
        p++;
    }

    if (base == 0) {
        if (*p == '0') {
            base = (*(p + 1) == 'x' || *(p + 1) == 'X') ? 16 : 8;
        } else {
            base = 10;
        }
    } else if (base == 16 && *p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
        p += 2;
    }

    while (*p) {
        int digit;
        if (isdigit(*p)) {
            digit = *p - '0';
        } else if (base == 16 && isxdigit(*p)) {
            digit = tolower(*p) - 'a' + 10;
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }

        result = result * base + digit;
        p++;
    }

    if (endptr) {
        *endptr = (char *)p;
    }

    return negative ? -result : result;
}
