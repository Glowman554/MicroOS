#include <emu.h>
#include <scheduler.h>
#include <string.h>
#include <time.h>
#include <poll.h>

void sys_async_getc(uc_engine *uc) {
    uint32_t c = 0;
    struct pollfd pfd = { .fd = STDIN_FILENO, .events = POLLIN };
    if (poll(&pfd, 1, 0) > 0) {
        unsigned char ch;
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            c = ch;
        }
    }

    if (c == 127) {
        c = '\b';
    }

    uc_reg_write(uc, UC_X86_REG_EBX, &c);
}

void sys_async_getarrw(uc_engine *uc) {
    uint32_t zero = 0;
    uc_reg_write(uc, UC_X86_REG_EBX, &zero);
}

void sys_vmode(uc_engine *uc) {
    uint32_t mode = 0; // TEXT_80x25
    uc_reg_write(uc, UC_X86_REG_EBX, &mode);
}

void sys_set_color(uc_engine *uc, uint32_t ebx) {
    char color[32];
    read_emu_string(uc, ebx, color, sizeof(color));
}

void sys_rgb_color(uint32_t ebx) {
}

void sys_vcursor_get(uc_engine *uc, uint32_t ebx, uint32_t ecx) {
    uint32_t zero = 0;
    uc_mem_write(uc, ebx, &zero, 4);
    uc_mem_write(uc, ecx, &zero, 4);
}

void sys_time(uc_engine *uc) {
    uint32_t t = (uint32_t)time(NULL);
    uc_reg_write(uc, UC_X86_REG_EBX, &t);
}

void sys_time_ms(uc_engine *uc) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint32_t ms = (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    uc_reg_write(uc, UC_X86_REG_EBX, &ms);
}



void sys_raminfo(uc_engine *uc) {
    uint32_t free_mem = 256 * 1024 * 1024;  // 256 MB
    uint32_t used_mem = 64 * 1024 * 1024;   // 64 MB
    uc_reg_write(uc, UC_X86_REG_EBX, &free_mem);
    uc_reg_write(uc, UC_X86_REG_ECX, &used_mem);
}

void sys_yield(uc_engine *uc) {
    uc_emu_stop(uc);
}

void sys_exit(uc_engine *uc, uint32_t ebx) {
    sched_exit_current((int)ebx);
}


