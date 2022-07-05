#pragma once

#include <interrupts/interrupts.h>

#include <config.h>

typedef cpu_registers_t* (*syscall_handler_t)(cpu_registers_t*);

void init_syscalls();

cpu_registers_t* sys_putc(cpu_registers_t* regs);