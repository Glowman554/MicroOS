#pragma once

#include <interrupts/interrupts.h>

#include <config.h>

typedef cpu_registers_t* (*syscall_handler_t)(cpu_registers_t*);

void init_syscalls();

cpu_registers_t* sys_open(cpu_registers_t* regs);
cpu_registers_t* sys_close(cpu_registers_t* regs);
cpu_registers_t* sys_read(cpu_registers_t* regs);
cpu_registers_t* sys_write(cpu_registers_t* regs);
cpu_registers_t* sys_filesize(cpu_registers_t* regs);

cpu_registers_t* sys_delete(cpu_registers_t* regs);
cpu_registers_t* sys_mkdir(cpu_registers_t* regs);
cpu_registers_t* sys_dir_at(cpu_registers_t* regs);
cpu_registers_t* sys_touch(cpu_registers_t* regs);
cpu_registers_t* sys_delete_dir(cpu_registers_t* regs);
cpu_registers_t* sys_fs_at(cpu_registers_t* regs);

cpu_registers_t* sys_async_getc(cpu_registers_t* regs);
cpu_registers_t* sys_exit(cpu_registers_t* regs);
cpu_registers_t* sys_mmap(cpu_registers_t* regs);

cpu_registers_t* sys_spawn(cpu_registers_t* regs);
cpu_registers_t* sys_get_proc_info(cpu_registers_t* regs);
cpu_registers_t* sys_yield(cpu_registers_t* regs);
cpu_registers_t* sys_env(cpu_registers_t* regs);
cpu_registers_t* sys_mmmap(cpu_registers_t* regs);

cpu_registers_t* sys_vmode(cpu_registers_t* regs);
cpu_registers_t* sys_vpoke(cpu_registers_t* regs);
