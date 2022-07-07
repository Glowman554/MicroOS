#include <scheduler/scheduler.h>

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <utils/string.h>
#include <utils/multiboot.h>
#include <scheduler/elf.h>
#include <fs/vfs.h>
#include <assert.h>


task_t tasks[MAX_TASKS] = { 0 };

task_t* init_task(void* entry) {
	task_t* task = NULL;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (!tasks[i].active) {
			task = &tasks[i];
			break;
		}
	}

	assert(task != NULL);

	memset(task, 0, sizeof(task_t));

	debugf("Creating task with entry point %p in task slot located at %p", entry, task);

	uint8_t* stack = pmm_alloc_range(KERNEL_STACK_SIZE_PAGES);
	uint8_t* user_stack = pmm_alloc_range(USER_STACK_SIZE_PAGES);

	cpu_registers_t new_state = {
		.eax = 0,
		.ebx = 0,
		.ecx = 0,
		.edx = 0,
		.esi = 0,
		.edi = 0,
		.ebp = 0,
		.esp = (uint32_t) user_stack + (USER_STACK_SIZE_PAGES * 4096) + USER_SPACE_OFFSET,
		.eip = (uint32_t) entry,

		.cs  = 0x18 | 0x03,
		.ss  = 0x20 | 0x03,

		.eflags = 0x202,
	};

	cpu_registers_t* state = (void*) (stack + KERNEL_STACK_SIZE_PAGES * 4096 - sizeof(new_state));
	*state = new_state;

	task->registers = state;
	task->active = true;
	task->stack = stack;
	task->user_stack = user_stack;

	task->context = vmm_create_context();
	vmm_clone_kernel_context(task->context);

	for (int i = 0; i < KERNEL_STACK_SIZE_PAGES; i++) {
		vmm_map_page(task->context, (uintptr_t) stack + i * 4096, (uintptr_t) stack + i * 4096, PTE_PRESENT | PTE_WRITE);
	}

	for (int i = 0; i < USER_STACK_SIZE_PAGES; i++) {
		vmm_map_page(task->context, (uintptr_t) user_stack + i * 4096 + USER_SPACE_OFFSET, (uintptr_t) user_stack + i * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER);
	}


	return task;
}

void init_elf(void* image) {

	struct elf_header* header = image;

	if (header->magic != ELF_MAGIC) {
		printf("ELF magic mismatch\n");
		return;
	}

	task_t* task = init_task((void*) header->entry);

	struct elf_program_header* ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (int i = 0; i < header->ph_entry_count; i++, ph++) {
		void* dest = (void*) ph->virt_addr;
		void* src = ((char*) image) + ph->offset;

		if (ph->type != 1) {
			continue;
		}

		void* phys_loc = pmm_alloc_range(ph->mem_size / 4096 + 1);
		for (int j = 0; j < ph->mem_size / 4096 + 1; j++) {
			vmm_map_page(task->context, (uintptr_t) dest + j * 4096, (uintptr_t) phys_loc + j * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER);
		}

		vmm_context_t old = vmm_get_current_context();
		vmm_activate_context(task->context);

		memset(dest, 0, ph->mem_size);
		memcpy(dest, src, ph->file_size);

		vmm_activate_context(&old);
	}
}


// IMPORTANT: This function must be called in ring 0
// IMPORTANT: This function should free every pmm allocation made in the pagemap witch isnt in the kernel context too
void exit_task(task_t* task) {
	asm volatile("cli");

	vmm_activate_context(kernel_context);
	vmm_destroy_context(task->context);

	task->active = false;

	debugf("Task %p exited", task);
	pmm_free_range((void*) task->stack, KERNEL_STACK_SIZE_PAGES);

	asm volatile("sti");
	while(1);
}

int current_task = 0;

bool is_scheduler_running = false;

cpu_registers_t* schedule(cpu_registers_t* registers, void* _) {
	if (!is_scheduler_running) {
		return registers;
	}

	for (int i = current_task + 1; i < MAX_TASKS; i++) {
		if(tasks[i].active) {
			current_task = i;
			vmm_activate_context(tasks[current_task].context);
			return tasks[current_task].registers;
		}
	}

	// we have no tasks left or we are at the end of the task list
	// so we start searching from 0 again

	for (int i = 0; i < MAX_TASKS; i++) {
		if(tasks[i].active) {
			current_task = i;
			vmm_activate_context(tasks[current_task].context);
			return tasks[current_task].registers;
		}
	}

	abortf("All tasks are dead lol\n");

	return registers;
}

void init_scheduler() {
	debugf("Initializing scheduler");

	// register_interrupt_handler(0x20, schedule, NULL); // now gets called by the interrupt handler of the pit timer

	file_t* file = vfs_open("initrd:/bin/terminal.elf", 0);
	assert(file != NULL);
	void* buffer = vmm_alloc(file->size / 4096 + 1);
	vfs_read(file, buffer, file->size, 0);
	init_elf(buffer);
	vmm_free(buffer, file->size / 4096 + 1);
	vfs_close(file);

	// file = vfs_open("initrd:/bin/test.elf", 0);
	// buffer = vmm_alloc(file->size / 4096 + 1);
	// vfs_read(file, buffer, file->size, 0);
	// init_elf(buffer);
	// vmm_free(buffer, file->size / 4096 + 1);
	// vfs_close(file);

	is_scheduler_running = true;
}
