#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/spawn.h>
#include <sys/env.h>

#define MAX_JOBS 128
#define CMD_LEN 256

#define MINUTE 60
#define HOUR   3600

// #define DEBUG

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

void parse_config(const char* file) {
#ifdef DEBUG
    printf("[scheduler] Parsing config file: %s\n", file);
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
        
        #ifdef DEBUG
            printf("[scheduler] Added reboot job: %s\n", j->command);
        #endif
        } else if (!strncmp(line, "every", 5)) {
            char unit[16] = { 0 };

            char* q = skip_ws(line + 5);
            q = read_int(q, &j->value);
            q = skip_ws(q);
            q = read_word(q, unit, sizeof(unit));

            j->type = (unit[0] == 'm') ? JOB_EVERY_MINUTES : JOB_EVERY_HOURS;

            read_rest(q, j->command, CMD_LEN);

        #ifdef DEBUG
            printf("[scheduler] Added every %d %s job: %s\n", j->value, unit,  j->command);
        #endif
        } else {
            continue;
        }

        j->last_run = 0;
        job_count++;
    }

    free(buf);
}

int main(int argc, char* argv[]) {
    parse_config("scheduler.conf");

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
                }
                break;

            case JOB_EVERY_HOURS:
                if (now - j->last_run >= j->value * HOUR) {
                    run_command(j->command);
                    j->last_run = now;
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
