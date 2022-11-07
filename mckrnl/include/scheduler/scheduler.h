#pragma once

#include <interrupts/interrupts.h>
#include <config.h>
#include <stdbool.h>
#include <memory/vmm.h>

typedef struct resource {
	void (*dealloc)(void* resource);
	void* resource;
} resource_t;

typedef struct {
	cpu_registers_t* registers;
	bool active;

	vmm_context_t* context;

	void* stack;
	void* user_stack;

	int pid;

	char** argv;
	char** envp;

	char pwd[128];

	resource_t* resources;
	int num_resources;
} task_t;

extern task_t tasks[MAX_TASKS];
extern int current_task;
extern bool is_scheduler_running;

cpu_registers_t* schedule(cpu_registers_t* registers, void* _);

task_t* init_task(void* entry);
int init_elf(void* image, char** argv, char** envp);
void exit_task(task_t* task);

task_t* get_task_by_pid(int pid);

void init_scheduler();

void resource_register_self(resource_t resource);
void resource_unregister_self(void* resource);
void resource_dealloc_self();


#define NOSHED(expr) { is_scheduler_running = false; asm volatile("sti"); expr; asm volatile("cli"); is_scheduler_running = true; }