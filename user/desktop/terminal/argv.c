#include <argv.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char* read_env_vars(char* in) {
    char tmp[512] = { 0 };
    int tmp_idx = 0;
    for (int i = 0; in[i] != '\0'; i++) {
        if (in[i] == '$') {
            char env_var[256] = { 0 };
            int env_var_idx = 0;
            i++;
            while (((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') || (in[i] >= '0' && in[i] <= '9') || in[i] == '_' || in[i] == '?') && in[i] != 0) {
                env_var[env_var_idx++] = in[i++];
            }
            char* val = getenv(env_var);
            if (val) {
                strcat(tmp, val);
                tmp_idx += strlen(val);
            }
            i--;
        } else {
            tmp[tmp_idx++] = in[i];
        }
    }
    char* out = malloc(strlen(tmp) + 1);
    memset(out, 0, strlen(tmp) + 1);
    memcpy(out, tmp, strlen(tmp) + 1);
    return out;
}

char** argv_split_simple(char* str) {
    int len = strlen(str);
    int argc = 1;
    bool in_quote = false;
    bool in_dquote = false;

    for (int i = 0; i < len; i++) {
        if (str[i] == ' ' && !in_quote && !in_dquote) {
            argc++;
        } else if (str[i] == '\"' && !in_dquote) {
            in_quote = !in_quote;
        } else if (str[i] == '\'' && !in_quote) {
            in_dquote = !in_dquote;
        }
    }

    char** argv = malloc(sizeof(char*) * (argc + 1));
    argc = 1;
    argv[0] = &str[0];
    in_quote = false;
    in_dquote = false;

    for (int i = 0; i < len; i++) {
        if (str[i] == ' ' && !in_quote && !in_dquote) {
            str[i] = 0;
            argv[argc++] = &str[i + 1];
        } else if (str[i] == '\"' && !in_dquote) {
            in_quote = !in_quote;
        } else if (str[i] == '\'' && !in_quote) {
            in_dquote = !in_dquote;
        }
    }
    argv[argc] = NULL;
    return argv;
}

char** argv_env_process_simple(char** in) {
    for (int i = 0; in[i] != NULL; i++) {
        in[i] = read_env_vars(in[i]);
    }
    return in;
}

void free_argv_simple(char** argv) {
    for (int i = 0; argv[i] != NULL; i++) {
        free(argv[i]);
    }
    free(argv);
}

char* process_line_simple(char* command) {
    int len = strlen(command);
    int start = 0;
    while (start < len && command[start] == ' ') {
        start++;
    }

    int end = len - 1;
    while (end > start && command[end] == ' ') {
        end--;
    }

    int new_len = end - start + 1;
    if (new_len <= 0) {
        command[0] = '\0';
        return command;
    }
    
    if (start > 0) {
        memmove(command, command + start, new_len);
    }
    command[new_len] = '\0';
    return command;
}
