#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <buildin/number_parser.h>

#define arg_str(name) char* name = NULL; for (int i = 1; i < argc; i++) { if (strcmp(argv[i], "--"#name) == 0) { name = argv[i + 1]; break; } }
#define arg(name) bool name = false; for (int i = 1; i < argc; i++) { if (strcmp(argv[i], "--"#name) == 0) { name = true; break; } }

char* remove_quotes(char* str) {
    if (str && str[0] == '"') {
        str[strlen(str) - 1] = '\0';
        return str + 1;
    }
    return str;
}

enum compare_type {
    EQUAL = 1,
    LESS,
    MORE
};

int parse_compare_type(char* str) {
    if (!str)
        return 0;

    if (strcmp(str, "==") == 0) {
        return EQUAL;
    } else if (strcmp(str, "<") == 0) {
        return LESS;
    } else if (strcmp(str, ">") == 0) {
        return MORE;
    }

    return 0;
}

char* strip_string(char* in) {
    if (!in) {
        return in;
    }

    size_t len = strlen(in);
    if (len && in[len - 1] == ' ') {
        in[len - 1] = '\0';
    }

    if (in[0] == ' ') {
        return in + 1;
    }

    return in;
}


bool file_exists(const char* path) {
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}


int main(int argc, char* argv[]) {
    arg_str(val1);
    arg_str(val2);
    arg_str(cmp);
    arg_str(cmd);
    arg_str(file);

    arg(not);

    /* ------------------ usage ------------------ */

    if (!file && (val1 == NULL || val2 == NULL || cmp == NULL)) {
        printf("Usage: %s [--val1 <str>] [--val2 <str>] [--cmp <==/</>>] [--file <path>] [--not] [--cmd <command>]\n", argv[0]);
        return 1;
    }

    if (cmd) {
        cmd = remove_quotes(cmd);
    } else {
        cmd = "echo command not set";
    }

    if (file) {
        bool exists = file_exists(file);

#ifdef IF_DEBUG
        printf("File check:\n");
        printf("  > file: %s\n", file);
        printf("  > exists: %s\n", exists ? "true" : "false");
        printf("  > not: %s\n", not ? "true" : "false");
        printf("  > cmd: %s\n", cmd);
#endif

        if ((exists && !not) || (!exists && not)) {
            system(cmd);
        }
        return 0;
    }

    int cmp_p = parse_compare_type(cmp);
    if (cmp_p == 0) {
        printf("Invalid compare type\n");
        return 1;
    }

    val1 = strip_string(val1);
    val2 = strip_string(val2);

#ifdef IF_DEBUG
    printf("Compare summary:\n");
    printf("  > val1: '%s'\n", val1);
    printf("  > val2: '%s'\n", val2);
    printf("  > cmp: %d\n", cmp_p);
    printf("  > cmd: %s\n", cmd);
    printf("  > not: %s\n", not ? "true" : "false");
#endif

    switch (cmp_p) {

        case EQUAL:
            if ((strcmp(val1, val2) == 0) ^ not) {
                system(cmd);
            }
            break;

        case LESS:
        {
            int num1, num2;
            __libc_parse_number(val1, &num1);
            __libc_parse_number(val2, &num2);
            if ((num1 < num2) ^ not) {
                system(cmd);
            }
            break;
        }

        case MORE:
        {
            int num1, num2;
            __libc_parse_number(val1, &num1);
            __libc_parse_number(val2, &num2);
            if ((num1 > num2) ^ not) {
                system(cmd);
            }
            break;
        }

        default:
            printf("This should never happen\n");
            return 1;
    }

    return 0;
}
