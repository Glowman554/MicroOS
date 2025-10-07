#include <launcher.h>
#include <stdio.h>
#include <sys/spawn.h>
#include <sys/mmap.h>
#include <sys/env.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <components/window.h>
#include <components/mouse.h>
#include <components/launcher.h>
#include <render.h>


const char** load_env(const char* envPath) {
    FILE* envFile = fopen(envPath, "r");
    if (!envFile) {
        // const char** envp = malloc(1 * sizeof(char*));
        // envp[0] = NULL;
        return NULL;
    }

    fsize(envFile, size);

    char* envStr = malloc(size + 1);
    envStr[size] = 0;
    fread(envStr, size, 1, envFile);
    fclose(envFile);
    
    const char* line = envStr;
    size_t envc = 0;
    const char** envp = NULL;

    while (line && *line) {
        const char* lineEnd = strchr(line, '\n');
        const char* lineCpy = strndup(line,  (uint32_t) lineEnd - (uint32_t) line);

        envc++;
        envp = realloc(envp, envc * sizeof(char*));
        envp[envc - 1] = lineCpy;

        line = lineEnd;
        if (line) {
            line++;
        }
    }
    
    envc++;
    envp = realloc(envp, envc * sizeof(char*));
    envp[envc - 1] = NULL;

    return envp;
}

void launch_child(const char* executable, const char* envFile, const char* icon, const char* argument) {
    const char* argv[] = {
        executable,
        argument,
        NULL
    };

    int window_slot = -1;
    for (int i = 0; i < max_instances; i++) {
        if (!window_instances[i].window) {
            window_slot = i;
            break;
        }
    }

    set_env(SYS_ENV_PIN, (void*) 1);

    const char** envp = load_env(envFile);
    int pid = -1;
    if (envp) {
        pid = spawn(executable, argv, envp);
        for (int i = 0; envp[i]; i++) {
            free((void*) envp[0]);
        }
        free(envp);
    } else {
    	const char** envp = (const char**) env(SYS_GET_ENVP_ID);
        pid = spawn(executable, argv, envp);
    }
    

    window_instances[window_slot].window = (window_t*) (0xF0002000 + (0x1000 * window_slot));
    window_instances[window_slot].pid = pid;
    mmmap(window_instances[window_slot].window, (void*) 0xF0002000, pid);

    memset(window_instances[window_slot].window, 0, sizeof(window_t));
    
    window_instances[window_slot].window->dirty = true;
    window_instances[window_slot].window->info = info;
    window_instances[window_slot].window->scale = SCALE;

    set_env(SYS_ENV_PIN, (void*) 0);

    while (!window_instances[window_slot].window->ready && get_proc_info(pid)) {
        yield();
    }

    if (!get_proc_info(pid)) {
        window_instances[window_slot].window = NULL;
        redraw_all();
        return;
    }

    window_instances[window_slot].icon = load_fpic(icon);

    unfocus_all();
    window_instances[window_slot].window->focus = true;
    redraw_all();
}

void cleanup_tasks() {
    for (int i = 0; i < max_instances; i++) {
        if (window_instances[i].window) {
            if (check_click_area(&window_instances[i].exit_button, &last_mouse)) {
                kill(window_instances[i].pid);
                yield();
            }
            if (!get_proc_info(window_instances[i].pid)) {
                window_instances[i].window = NULL;
                free(window_instances[i].icon);
                redraw_all();
            }
        }
    }
}