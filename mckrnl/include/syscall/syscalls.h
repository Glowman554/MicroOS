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
cpu_registers_t* sys_async_getc(cpu_registers_t* regs);
cpu_registers_t* sys_exit(cpu_registers_t* regs);
cpu_registers_t* sys_mmap(cpu_registers_t* regs);
