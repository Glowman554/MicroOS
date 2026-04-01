#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <non-standard/sys/time.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/sys/env.h>
#include <non-standard/sys/file.h>
#include <non-standard/buildin/data/array.h>
#include <non-standard/buildin/path.h>
#include <assert.h>

#define MAX_JOBS 128
#define CMD_LEN 256

#define MINUTE 60
#define HOUR   3600

#define DEBUG_SCHEDULER

typedef enum {
    JOB_EVERY_MINUTES,
    JOB_EVERY_HOURS,
    JOB_REBOOT
} job_type_t;

typedef struct {
    job_type_t type;
    int value;
    int hour, minute;
    char command[CMD_LEN];
    long last_run;
    int last_run_day;
} job_t;

job_t jobs[MAX_JOBS];
int job_count = 0;

#define MAX_CMD 64
#define MAX_PWD 64
typedef struct {
    char key;
    char launcher[MAX_CMD];
    char command[MAX_CMD];
    char pwd[MAX_PWD];
} shortcut_t;

char* skip_ws(char* p) {
    while (*p == ' ' || *p == '\t') {
        p++;
    }
    return p;
}

char* read_int(char* p, int* out) {
    int v = 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        p++;
    }
    *out = v;
    return p;
}

char* read_word(char* p, char* out, size_t max) {
    size_t i = 0;
    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && i + 1 < max) {
        out[i++] = *p++;
    }
    out[i] = 0;
    return p;
}

void read_rest(char* p, char* out, size_t max) {
    p = skip_ws(p);
    size_t i = 0;
    while (*p && *p != '\n' && i + 1 < max) {
        out[i++] = *p++;
    }
    out[i] = 0;
}


void run_command(const char* cmd) {
    system((char*) cmd);
}

shortcut_t* parse_config(const char* file) {
#ifdef DEBUG_SCHEDULER
    printf("scheduler: Parsing config file: %s\n", file);
#endif

    FILE* f = fopen(file, "rb");
    if (!f) {
        printf("Failed to open config file: %s\n", file);
        abort();
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        abort();
    }

    fread(buf, 1, size, f);
    buf[size] = 0;
    fclose(f);

    shortcut_t* shortcuts = array_create(sizeof(shortcut_t), 4);

    char* p = buf;

    while (*p) {
        char* line = p;
        while (*p && *p != '\n') {
            p++;
        }
        if (*p) {
            *p++ = 0;
        }

        line = skip_ws(line);
        if (*line == 0 || *line == '#') {
            continue;
        }

        job_t* j = &jobs[job_count];
        memset(j, 0, sizeof(*j));

        if (!strncmp(line, "@reboot", 7)) {
            j->type = JOB_REBOOT;
            read_rest(line + 7, j->command, CMD_LEN);
        
        #ifdef DEBUG_SCHEDULER
            printf("scheduler: Added reboot job: %s\n", j->command);
        #endif
        } else if (!strncmp(line, "every", 5)) {
            char unit[16] = { 0 };

            char* q = skip_ws(line + 5);
            q = read_int(q, &j->value);
            q = skip_ws(q);
            q = read_word(q, unit, sizeof(unit));

            j->type = (unit[0] == 'm') ? JOB_EVERY_MINUTES : JOB_EVERY_HOURS;

            read_rest(q, j->command, CMD_LEN);

        #ifdef DEBUG_SCHEDULER
            printf("scheduler: Added every %d %s job: %s\n", j->value, unit,  j->command);
        #endif
        } else if (!strncmp(line, "shortcut", 8)) {
            char key[32] = { 0 };
            char* q = read_word(line + 9, key, sizeof(key));

            char command[MAX_CMD] = { 0 };
            read_rest(q + 1, command, sizeof(command));
            
        #ifdef DEBUG_SCHEDULER
            printf("scheduler: Shortcut '%s' -> '%s'\n", key, command);
        #endif

            // make sure key starts with Strg+
            if (strncmp(key, "Strg+", 5) != 0) {
                printf("scheduler: Invalid shortcut key '%s', must start with 'Strg+'\n", key);
                continue;
            }

            shortcut_t sc = { 0 };
            sc.key = key[5];
            strcpy(sc.command, command);
            array_push(shortcuts, &sc);

            continue;
        } else {
            continue;
        }

        j->last_run = 0;
        job_count++;
    }

    printf("scheduler: Total jobs loaded: %d\n", job_count);
    printf("scheduler: Total shortcuts loaded: %d\n", array_length(shortcuts));

    free(buf);
    return shortcuts;
}

int main(int argc, char* argv[]) {
    shortcut_t* shortcuts = parse_config("scheduler.conf");

    if (array_length(shortcuts) > 0) {
        int fd = open("dev:shortcut", 0);
        if (fd < 0) {
            printf("scheduler: Failed to open shortcut interface\n");
        } else {
            char* terminal = search_executable("terminal");
            if (!terminal) {
                printf("scheduler: Failed to find terminal executable for shortcuts\n");
                abort();
            }

            char* root_fs = getenv("ROOT_FS");
            assert(root_fs);

            for (int i = 0; i < array_length(shortcuts); i++) {
                strcpy(shortcuts[i].launcher, terminal);
                strcpy(shortcuts[i].pwd, root_fs);
            }

            free(terminal);

            size_t size = array_length(shortcuts) * sizeof(shortcut_t);
            write(fd, shortcuts, size, 0);
            close(fd);
            array_destroy(shortcuts);

            printf("scheduler: Shortcuts registered with the system\n");
        }
    } else {
        array_destroy(shortcuts);
    }

    for (int i = 0; i < job_count; i++) {
        if (jobs[i].type == JOB_REBOOT) {
            run_command(jobs[i].command);
            jobs[i].last_run = time(NULL);
        }
    }

    while (1) {
        long now = time(NULL);

        for (int i = 0; i < job_count; i++) {
            job_t* j = &jobs[i];

            switch (j->type) {

            case JOB_EVERY_MINUTES:
                if (now - j->last_run >= j->value * MINUTE) {
                    run_command(j->command);
                    j->last_run = now;
                } else {
                #ifdef DEBUG_SCHEDULER
                    printf("scheduler: Remaining time until execution '%s': %d seconds\n", j->command, j->value * MINUTE - (now - j->last_run));
                #endif
                }
                break;

            case JOB_EVERY_HOURS:
                if (now - j->last_run >= j->value * HOUR) {
                    run_command(j->command);
                    j->last_run = now;
                } else {
                #ifdef DEBUG_SCHEDULER
                    printf("scheduler: Remaining time until execution '%s': %d seconds\n", j->command, j->value * HOUR - (now - j->last_run));
                #endif
                }
                break;

            default:
                break;
            }
        }

        set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*) (1000 * 60));
        yield();
    }
}
