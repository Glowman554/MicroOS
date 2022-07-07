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
extern int current_task;

cpu_registers_t* schedule(cpu_registers_t* registers, void* _);

task_t* init_task(void* entry);
void exit_task(task_t* task);

void init_scheduler();