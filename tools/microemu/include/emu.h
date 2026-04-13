#pragma once

#include <syscalls.h>
#include <fs/vfs.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <unicorn/unicorn.h>

#define ARGS_ADDR  0x20000000
#define ARGS_SIZE  0x00100000

#define STACK_ADDR 0x0ff00000
#define STACK_SIZE 0x00100000



#define fatalf(...) do { \
    fprintf(stderr, __VA_ARGS__); \
} while (0)

extern const char *testcmd;
extern int testcmd_pos;
extern uint32_t argv_addr;
extern uint32_t envp_addr;
extern char emu_pwd[512];

void read_emu_string(uc_engine *uc, uint32_t addr, char *buf, size_t max);

void sys_open(uc_engine *uc, uint32_t ebx, uint32_t ecx);
void sys_close(uint32_t ebx);
void sys_read(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi);
void sys_write(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi);
void sys_filesize(uc_engine *uc, uint32_t ebx);
void sys_delete(uint32_t ebx);
void sys_mkdir(uc_engine *uc, uint32_t ebx);
void sys_dir_at(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx);
void sys_touch(uc_engine *uc, uint32_t ebx);
void sys_delete_dir(uc_engine *uc, uint32_t ebx);
void sys_fs_at(uc_engine *uc, uint32_t ebx, uint32_t ecx);
void sys_truncate(uint32_t ebx, uint32_t ecx);

void sys_env(uc_engine *uc, uint32_t ebx, uint32_t ecx);

void sys_mmap(uc_engine *uc, uint32_t ebx);
void sys_mmap_mapped(uc_engine *uc, uint32_t ebx);

void sys_async_getc(uc_engine *uc);
void sys_async_getarrw(uc_engine *uc);
void sys_vmode(uc_engine *uc);
void sys_set_color(uc_engine *uc, uint32_t ebx);
void sys_rgb_color(uint32_t ebx);
void sys_vcursor_get(uc_engine *uc, uint32_t ebx, uint32_t ecx);
void sys_time(uc_engine *uc);
void sys_time_ms(uc_engine *uc);
void sys_raminfo(uc_engine *uc);
void sys_yield(uc_engine *uc);
void sys_exit(uc_engine *uc, uint32_t ebx);

void sys_spawn(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx);
void sys_get_proc_info(uc_engine *uc, uint32_t ebx);
void sys_kill(uc_engine *uc, uint32_t ebx);
void sys_get_exit_code(uc_engine *uc, uint32_t ebx);

void sys_set_term(uint32_t ebx, uint32_t ecx);

void sys_message_send(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx);
void sys_message_recv(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx);
