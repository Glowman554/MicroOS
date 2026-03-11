#include "commands.h"
#include "desktop.h"
#include "output.h"
#include "argv.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/file.h>
#include <buildin/path.h>

#define PIPE_BUFFER_SIZE 65536

void builtin_cd(terminal_state_t* st, char** argv) {
    int argc = 0;
    while (argv[argc]) {
        argc++;
    }

    char path_buf[256] = { 0 };
    bool cancd = false;

    if (argc == 1) {
        char* root = getenv("ROOT_FS");
        if (root) {
            cancd = resolve(root, path_buf);
        } else {
            term_puts(st, "No root filesystem specified!\n");
            return;
        }
    } else if (argc == 2) {
        cancd = resolve(argv[1], path_buf);
    } else {
        term_puts(st, "Too many arguments.\n");
        return;
    }

    if (!cancd) {
        term_printf(st, "No such file or directory: %s\n", path_buf);
        return;
    }

    int fd = open(path_buf, FILE_OPEN_MODE_READ);
    if (fd != -1) {
        term_puts(st, "You can only change to a folder!\n");
        close(fd);
        return;
    }

    set_env(SYS_SET_PWD_ID, path_buf);
}

void builtin_pwd(terminal_state_t* st) {
    char cwd[64] = { 0 };
    set_env(SYS_GET_PWD_ID, cwd);
    term_printf(st, "%s\n", cwd);
}

void builtin_open(terminal_state_t* st, char* path) {
    char path_buf[256] = { 0 };
    if (!resolve(path, path_buf)) {
        term_printf(st, "Could not resolve path: %s\n", path);
        return;
    }

    desktop_open_file(path_buf);
}

void builtin_export(terminal_state_t* st, char* cmd) {
    char* env_str = read_env_vars(cmd);
    if (strlen(env_str) <= 7) {
        term_puts(st, "No environment variable specified! Try: export MYVAR=value\n");
        free(env_str);
        return;
    }
    char* env_var = env_str + 7;

    char* tmp = strdup(env_var);
    char* name = strtok(tmp, "=");
    if (strcmp(env_var, name) == 0) {
        term_puts(st, "No value specified! Try: export MYVAR=value\n");
        free(tmp);
        free(env_str);
        return;
    }
    size_t name_len = strlen(name);

    char** envp = st->envp;
    bool found = false;
    int next = 0;
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(name, envp[i], name_len) == 0 && envp[i][name_len] == '=') {
            found = true;
            free(envp[i]);
            envp[i] = strdup(env_var);
            break;
        }
        next = i + 1;
    }
    if (!found) {
        envp[next] = strdup(env_var);
        envp[next + 1] = NULL;
    }

    free(tmp);
    free(env_str);
}

void builtin_layout(terminal_state_t* st, char* cmd) {
    if (cmd[7] == 0) {
        term_puts(st, "No keyboard layout specified!\n");
    } else {
        char* keymap = &cmd[7];
        term_printf(st, "Setting keyboard layout to %s\n", keymap);
        set_env(SYS_ENV_SET_LAYOUT, keymap);
    }
}


void set_wait_and_yield_term() {
    set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)100);
    yield();
}

int spawn_and_capture(terminal_state_t* st, char** argv) {
    char* executable = search_executable(argv[0]);
    if (!executable) {
        return -1;
    }

    pipe_t stdout_pipe;
    stdout_pipe.buffer = malloc(PIPE_BUFFER_SIZE);
    stdout_pipe.size = PIPE_BUFFER_SIZE;
    stdout_pipe.pos = 0;
    memset(stdout_pipe.buffer, 0, PIPE_BUFFER_SIZE);

    const char** envp = (const char**)st->envp;

    set_env(SYS_ENV_PIN, (void*)1);
    yield();
    int pid = spawn(executable, (const char**)argv, envp);

    if (pid == -1) {
        set_env(SYS_ENV_PIN, (void*)0);
        free(stdout_pipe.buffer);
        free(executable);
        return -1;
    }

    set_pipe(pid, &stdout_pipe, PIPE_STDOUT);
    set_env(SYS_ENV_PIN, (void*)0);

    while (get_proc_info(pid)) {
        set_wait_and_yield_term();
    }

    if (stdout_pipe.pos > 0) {
        term_append(st, stdout_pipe.buffer, stdout_pipe.pos);
    }

    free(stdout_pipe.buffer);
    free(executable);
    return pid;
}


bool execute_command(terminal_state_t* st, char* command) {
    char* cmd = process_line_simple(command);
    if (cmd[0] == '\0') {
        return true;
    }

    if (strncmp(cmd, "layout ", 7) == 0) {
        builtin_layout(st, cmd);
    } else if (strncmp(cmd, "cd ", 3) == 0 || strcmp(cmd, "cd") == 0) {
        char* env_cmd = read_env_vars(cmd);
        char** argv = argv_split_simple(env_cmd);
        builtin_cd(st, argv);
        free(argv);
        free(env_cmd);
    } else if (strncmp(cmd, "pwd", 3) == 0) {
        builtin_pwd(st);
    } else if (strncmp(cmd, "export ", 7) == 0) {
        builtin_export(st, cmd);
    } else if (strcmp(cmd, "exit") == 0) {
        return false;
    } else if (strcmp(cmd, "clear") == 0) {
        st->output_len = 0;
        st->output_buf[0] = '\0';
        st->scroll_offset = 0;
    } else if (strncmp(cmd, "open ", 5) == 0) {
        char* env_cmd = read_env_vars(cmd);
        builtin_open(st, env_cmd + 5);
        free(env_cmd);
    } else {
        char* env_cmd = read_env_vars(cmd);
        char** argv = argv_split_simple(env_cmd);
        argv = argv_env_process_simple(argv);

        int pid = spawn_and_capture(st, argv);
        if (pid == -1) {
            term_printf(st, "Error: Command not found: '%s'\n", argv[0]);
        }

        free_argv_simple(argv);
        free(env_cmd);
    }
    return true;
}
