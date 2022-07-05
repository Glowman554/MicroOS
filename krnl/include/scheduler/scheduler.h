#pragma once

#include <interrupts/interrupts.h>
#include <config.h>
#include <stdbool.h>
#include <memory/vmm.h>


typedef struct {
	cpu_registers_t* registers;
	bool active;

	vmm_context_t* context;

	void* stack;
	void* user_stack;
} task_t;

extern task_t tasks[MAX_TASKS];


task_t* init_task(void* entry);

cpu_registers_t* schedule(cpu_registers_t* registers);

void init_scheduler();