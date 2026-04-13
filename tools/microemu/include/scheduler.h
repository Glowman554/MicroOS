#pragma once

#include <unicorn/unicorn.h>
#include <stdint.h>
#include <stdbool.h>
#include <elf.h>

#define MAX_TASKS 64
#define SCHED_SLICE 200000
#define MAX_EXIT_CODES 256

typedef struct {
    uc_engine *uc;
    uc_hook hook;
    int pid;
    bool active;
    bool stopped;
    int exit_code;
    uint32_t entry;

    uint32_t argv_addr;
    uint32_t envp_addr;
    char pwd[512];
} emu_proc_t;

extern emu_proc_t procs[MAX_TASKS];
extern int current_pid;

void sched_init(void);

int sched_spawn(uc_engine *parent_uc, uint32_t path_addr, uint32_t argv_addr, uint32_t envp_addr);

int sched_load(const char *vfs_path, int host_argc, char **host_argv);

int sched_run(void);

emu_proc_t *sched_get_proc(int pid);

emu_proc_t *sched_current(void);

void sched_exit_current(int code);

int sched_get_exit_code(int pid);
