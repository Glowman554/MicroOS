#include <scheduler/scheduler.h>

#include <stdint.h>
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
#include <driver/apic/lapic.h>
#include <driver/apic/smp.h>
#include <driver/timer_driver.h>
#include <driver/char_output_driver.h>


int current_pid = 0;
task_t tasks[MAX_TASKS] = { 0 };

task_t* init_task(int term, void* entry, bool thread, task_t* parent) {	
	bool old_shed = is_scheduler_running;
	is_scheduler_running = false;

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

	uint8_t* stack = vmm_alloc(KERNEL_STACK_SIZE_PAGES);
	uint8_t* user_stack = vmm_alloc(USER_STACK_SIZE_PAGES);

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

		.gs  = 0,
		.fs  = 0,
		.es  = 0x20 | 0x03,
		.ds  = 0x20 | 0x03,

		.eflags = 0x202,
	};

	cpu_registers_t* state = (void*) (stack + KERNEL_STACK_SIZE_PAGES * 4096 - sizeof(new_state));
	*state = new_state;

	task->registers = state;
	task->active = true;
	task->stack = stack;
	task->user_stack = user_stack;
	task->pid = current_pid++;
	task->wait_time = 0;
	task->term = term;

	if (!thread) {
		task->context = vmm_create_context();
		vmm_clone_kernel_context(task->context);
		task->parent = -1;
	} else {
		task->context = parent->context;
		task->parent = parent->pid;
	}

	for (int i = 0; i < KERNEL_STACK_SIZE_PAGES; i++) {
		// vmm_map_page(task->context, (uintptr_t) stack + i * 4096, (uintptr_t) stack + i * 4096, PTE_PRESENT | PTE_WRITE);
	}

	for (int i = 0; i < USER_STACK_SIZE_PAGES; i++) {
		vmm_map_page(task->context, (uintptr_t) user_stack + i * 4096 + USER_SPACE_OFFSET, (uintptr_t) user_stack + i * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER);
	}

	is_scheduler_running = old_shed;

	return task;
}

int init_elf(int term, void* image, char** argv, char** envp) {

	struct elf_header* header = image;

	if (header->magic != ELF_MAGIC) {
		printf("ELF magic mismatch\n");
		return -1;
	}

	task_t* task = init_task(term, (void*) header->entry, false, NULL);

	struct elf_program_header* ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (int i = 0; i < header->ph_entry_count; i++, ph++) {
		void* dest = (void*) ph->virt_addr;
		void* real_dest = (void*) ALIGN_PAGE_DOWN((uintptr_t) dest);

		void* src = ((char*) image) + ph->offset;

		if (ph->type != 1) {
			continue;
		}    

		int real_size = ph->mem_size / 4096 + 1;
		if (dest != real_dest) {
			real_size++;
		}

		void* phys_loc = pmm_alloc_range(real_size);
		for (int j = 0; j < real_size; j++) {
			if (vmm_lookup((uintptr_t) real_dest + j * 4096, task->context)) {
				pmm_free(phys_loc + j * 4096);
			} else {
				vmm_map_page(task->context, (uintptr_t) real_dest + j * 4096, (uintptr_t) phys_loc + j * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER);
			}
		}

		vmm_context_t old = vmm_get_current_context();
		vmm_activate_context(task->context);

		memset(dest, 0, ph->mem_size);
		memcpy(dest, src, ph->file_size);

		vmm_activate_context(&old);
	}

	int num_envp = 0;
	for (num_envp = 0; envp[num_envp] != NULL; num_envp++);

	int num_argv = 0;
	for (num_argv = 0; argv[num_argv] != NULL; num_argv++);

	debugf("copying %d arguments and %d environment variables", num_argv, num_envp);

	task->argv = (char**) vmm_alloc(1);
	vmm_map_page(task->context, (uintptr_t) task->argv + USER_SPACE_OFFSET, (uintptr_t) task->argv, PTE_PRESENT | PTE_WRITE | PTE_USER);

	for (int i = 0; i < num_argv; i++) {
		task->argv[i] = (char*) vmm_alloc(1);
		vmm_map_page(task->context, (uintptr_t) task->argv[i] + USER_SPACE_OFFSET, (uintptr_t) task->argv[i], PTE_PRESENT | PTE_WRITE | PTE_USER);
		memset(task->argv[i], 0, 0x1000);
		strcpy(task->argv[i], argv[i]);
	}

	task->argv[num_argv] = NULL;

	task->envp = (char**) vmm_alloc(1);
	vmm_map_page(task->context, (uintptr_t) task->envp + USER_SPACE_OFFSET, (uintptr_t) task->envp, PTE_PRESENT | PTE_WRITE | PTE_USER);

	for (int i = 0; i < num_envp; i++) {
		task->envp[i] = (char*) vmm_alloc(1);
		vmm_map_page(task->context, (uintptr_t) task->envp[i] + USER_SPACE_OFFSET, (uintptr_t) task->envp[i], PTE_PRESENT | PTE_WRITE | PTE_USER);
		memset(task->envp[i], 0, 0x1000);
		strcpy(task->envp[i], envp[i]);
	}

	task->envp[num_envp] = NULL;

	for (int i = 0; i < num_argv; i++) {
		task->argv[i] = (char*) ((uint32_t) task->argv[i] + USER_SPACE_OFFSET);
	}

	task->argv = (char**) ((uint32_t) task->argv + USER_SPACE_OFFSET);

	for (int i = 0; i < num_envp; i++) {
		task->envp[i] = (char*) ((uint32_t) task->envp[i] + USER_SPACE_OFFSET);
	}

	task->envp = (char**) ((uint32_t) task->envp + USER_SPACE_OFFSET);

	return task->pid;
}

void exit_task(task_t* task) {
	asm volatile("cli");

	task_t* self = get_self();

	task->active = false;
	task->pin = false;

	resource_dealloc(task);
	if (task->parent == -1) {
		vmm_activate_context(kernel_context);
		vmm_destroy_context(task->context);
	}

	debugf("Task %p (%d) exited", task, task->pid);
	vmm_free((void*) task->stack, KERNEL_STACK_SIZE_PAGES);

	if (task == self) {
		asm volatile("sti");
		asm volatile("hlt");
	}
}

int current_task = 0;
int last_time_ms = 0;
bool is_scheduler_running = false;

cpu_registers_t* switch_to_next_task(int starting_from, int diff_ms, bool ignore_wait_time) {
    for (int i = starting_from; i < MAX_TASKS; i++) {
		if (!ignore_wait_time && tasks[i].active && tasks[i].wait_time > 0) {
			tasks[i].wait_time -= diff_ms;
			if (tasks[i].wait_time > 0) {
				continue;
            }
		}
		
		if(tasks[i].active) {
			current_task = i;
			vmm_activate_context(tasks[current_task].context);
			return tasks[current_task].registers;
		}
	}
    return NULL;
}

cpu_registers_t* schedule(cpu_registers_t* registers, void* _) {
	if (!is_scheduler_running) {
		return registers;
	}

	LAPIC_ID(core_id);
	if (core_id != bsp_id) {
		return registers;
	}

	if (tasks[current_task].pin && tasks[current_task].term == global_char_output_driver->current_term) {
		return tasks[current_task].registers;
	}

	int curr_time_ms = global_timer_driver->time_ms(global_timer_driver);
	int diff_ms = curr_time_ms - last_time_ms;
	last_time_ms = curr_time_ms;

    cpu_registers_t* new_state;
    if ((new_state = switch_to_next_task(current_task + 1, diff_ms, false))) {
        return new_state;
    }

    // we have no tasks left or we are at the end of the task list
	// so we start searching from 0 again
    if ((new_state = switch_to_next_task(0, diff_ms, false))) {
        return new_state;
    }

	// no task is really busy rn, so we just pick the first active one even if its in timeout lol
    if ((new_state = switch_to_next_task(0, diff_ms, true))) {
        return new_state;
    }

	abortf("All tasks are dead lol\n");

	return registers;
}

void init_scheduler() {
	debugf("Initializing scheduler");
 
	// register_interrupt_handler(0x20, schedule, NULL); // now gets called by the interrupt handler of the pit timer

	// file = vfs_open("initrd:/bin/test.elf", 0);
	// buffer = vmm_alloc(file->size / 4096 + 1);
	// vfs_read(file, buffer, file->size, 0);
	// init_elf(buffer);
	// vmm_free(buffer, file->size / 4096 + 1);
	// vfs_close(file);

	is_scheduler_running = true;
}


task_t* get_task_by_pid(int pid) {
	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].active && tasks[i].pid == pid) {
			return &tasks[i];
		}
	}

	return NULL;
}

task_t* get_self() {
	return &tasks[current_task];
}

int read_task_list(task_list_t* out, int max) {
	int j = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].active) {
			char* argv;
			vmm_read_context(tasks[i].argv, &argv, sizeof(char*), tasks[i].context);
			vmm_read_context(argv, out[j].name, sizeof(out[j].name), tasks[i].context);
			out[j].term = tasks[i].term;
			out[j++].pid = tasks[i].pid;

			if (j >= max) {
				return j;
			}
		}
	}
	return j;
}

int get_ammount_running_tasks() {
    int j = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].active) {
            j++;
        }
    }
    return j;
}