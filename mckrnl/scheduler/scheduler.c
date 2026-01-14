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
#include <driver/acpi/madt.h>
#include <driver/char_output_driver.h>
#include <utils/tinf.h>
#include <utils/lock.h>

extern uint8_t idle_task[];

int current_pid = 0;

#ifdef SMP
task_t tasks[MAX_CPU][MAX_TASKS] = { 0 };
int current_task[MAX_CPU] = { 0 };
int last_time_ms[MAX_CPU] = { 0 };
#else
task_t tasks[MAX_TASKS] = { 0 };
int current_task = 0;
int last_time_ms = 0;
#endif

#ifdef SMP
	#define c_task(x) tasks[core_id][x]
	#define c_current_task current_task[core_id]
	#define c_last_time_ms last_time_ms[core_id]
#else
	#define c_task(x) tasks[x]
	#define c_current_task current_task
	#define c_last_time_ms last_time_ms
#endif

bool is_scheduler_running = false;

#ifdef SMP
int find_least_loaded_core(void) {
    int min_tasks = MAX_TASKS + 1;
    int least_loaded_core = -1;

    for (int cpu = 0; cpu < madt_lapic_count; cpu++) {
        int count = 0;
        for (int i = 0; i < MAX_TASKS; i++) {
            if (tasks[cpu][i].active) {
                count++;
            }
        }

		if (count < min_tasks) {
            min_tasks = count;
            least_loaded_core = cpu;
        }
    }

    return least_loaded_core;
}
#endif

task_t* find_empty_task_slot() {
#ifdef SMP
	int core_id = find_least_loaded_core();
	debugf("Selected core %d", core_id);
#endif

	for (int i = 0; i < MAX_TASKS; i++) {
		if (!c_task(i).taken) {   
			return &c_task(i);
		}
	}

	abortf(false, "Failed to find empty task slot");
	return NULL;
}

define_spinlock(init_task_lock);
task_t* init_task(int term, void* entry, bool thread, task_t* parent) {	
	atomic_acquire_spinlock(init_task_lock);

	bool old_shed = is_scheduler_running;
	is_scheduler_running = false;

	task_t* task = find_empty_task_slot();

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
	task->taken = true;
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

	atomic_release_spinlock(init_task_lock);

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

	task->active = true;

	return task->pid;
}

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

int init_mex(int term, void* image, char** argv, char** envp) {
	mex_header_t* header = image;
	void* content = (void*) header + sizeof(mex_header_t);

	if (memcmp(header->header, "MEX", 4) != 0) {
		printf("MEX magic mismatch\n");
		return -1;
	}

	unsigned int decompressed_size = read_le32(content + header->elfSizeCompressed - 4);
	debugf("decompressing %dkb to %dkb", header->elfSizeCompressed / 1024, decompressed_size / 1024);
	char* dest = (char*) vmm_alloc(TO_PAGES(decompressed_size));
	unsigned int output_size = decompressed_size;
	int res = tinf_gzip_uncompress(dest, &output_size, content, header->elfSizeCompressed);
		
	if ((res != TINF_OK) || (output_size != decompressed_size)) {
		printf("decompression failed: ");
		switch (res) {
			case TINF_DATA_ERROR:
				printf("TINF_DATA_ERROR\n");
				break;
			case TINF_BUF_ERROR:
				printf("TINF_BUF_ERROR\n");
				break;
			default:
				printf("Unknown error\n");
				break;
		}
		vmm_free(dest, TO_PAGES(decompressed_size));
		return -1;
	}

	int pid = init_elf(term, dest, argv, envp);
	vmm_free(dest, TO_PAGES(decompressed_size));
	return pid;
}

int init_executable(int term, void* image, char** argv, char** envp) {
	debugf("Loading executable at %p", image);
	
	for (int i = 0; argv[i] != NULL; i++) {
		debugf("argv[%d]: %s", i, argv[i]);
	}

	for (int i = 0; envp[i] != NULL; i++) {
		debugf("envp[%d]: %s", i, envp[i]);
	}

	mex_header_t* header = image;
	if (memcmp(header->header, "MEX", 4) == 0) {
		return init_mex(term, image, argv, envp);
	}

	return init_elf(term, image, argv, envp);
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

	task->taken = false;

	if (task == self) {
		asm volatile("sti");
		asm volatile("hlt");
	} else {
		vmm_activate_context(self->context);
	}
}

cpu_registers_t* switch_to_next_task(int starting_from, int diff_ms, bool ignore_wait_time) {
#ifdef SMP
	int core_id = lapic_id();
#endif

    for (int i = starting_from; i < MAX_TASKS; i++) {
		if (!ignore_wait_time && c_task(i).active && c_task(i).wait_time > 0) {
			c_task(i).wait_time -= diff_ms;
			if (c_task(i).wait_time > 0) {
				continue;
            }
		}
		
		if(c_task(i).active) {
			c_current_task = i;
			vmm_activate_context(c_task(c_current_task).context);
			return c_task(c_current_task).registers;
		}
	}
    return NULL;
}

void interrupt_cores() {
#ifdef SMP
	int id = lapic_id();
	for (int i = 0; i < madt_lapic_count; i++) {
		if (i != id) {
			lapic_ipi(madt_lapic_ids[i], 0x20);
		}
	}
#endif
}

cpu_registers_t* schedule(cpu_registers_t* registers, void* _) {
	if (!is_scheduler_running) {
		return registers;
	}

	int core_id = lapic_id();

#ifndef SMP
	if (core_id != bsp_id) {
		return registers;
	}
#endif


	if (c_task(c_current_task).pin && c_task(c_current_task).term == global_char_output_driver->current_term) {
		interrupt_cores();
		return c_task(c_current_task).registers;
	}

	int curr_time_ms = global_timer_driver->time_ms(global_timer_driver);
	int diff_ms = curr_time_ms - c_last_time_ms;
	c_last_time_ms = curr_time_ms;

    cpu_registers_t* new_state;
    if ((new_state = switch_to_next_task(c_current_task + 1, diff_ms, false))) {
		interrupt_cores();
        return new_state;
    }

    // we have no tasks left or we are at the end of the task list
	// so we start searching from 0 again
    if ((new_state = switch_to_next_task(0, diff_ms, false))) {
		interrupt_cores();
        return new_state;
    }

	// no task is really busy rn, so we just pick the first active one even if its in timeout lol
    if ((new_state = switch_to_next_task(0, diff_ms, true))) {
		interrupt_cores();
        return new_state;
    }

	abortf(false, "All tasks are dead lol\n");

	return registers;
}

void init_scheduler() {
	debugf("Initializing scheduler");
 
	// register_interrupt_handler(0x20, schedule, NULL); // now gets called by the interrupt handler of the pit timer

#ifdef SMP // we do not need a idle task with only once core. The init process is the idle task in this case
	for (int i = 0; i < madt_lapic_count; i++) {
		char* argv[] = { "(idle)", NULL };
		char* envp[] = { NULL };
		init_elf(1, idle_task, argv, envp);
	}
#endif

	is_scheduler_running = true;
}


task_t* get_task_by_pid(int pid) {
#ifdef SMP
	for (int i = 0; i < MAX_CPU; i++) {
		for (int j = 0; j < MAX_TASKS; j++) {
			if (tasks[i][j].active && tasks[i][j].pid == pid) {
				return &tasks[i][j];
			}
		}
	}
#else
	for (int i = 0; i < MAX_TASKS; i++) {
		for (int i = 0; i < MAX_TASKS; i++) {
			if (tasks[i].active && tasks[i].pid == pid) {
				return &tasks[i];
			}
		}
	}
#endif

	return NULL;
}

task_t* get_self() {
#ifdef SMP
	int id = lapic_id();
	return &tasks[id][current_task[id]];
#else
	return &tasks[current_task];
#endif
}

int read_task_list(task_list_t* out, int max) {
#ifdef SMP
	todo();
	return 0;
#else
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
#endif
}

int get_amount_running_tasks() {
#ifdef SMP
    int k = 0;
	for (int i = 0; i < MAX_CPU; i++) {
		for (int j = 0; j < MAX_TASKS; j++) {
			if (tasks[i][j].active) {
				k++;
			}
		}
    }
    return k;
#else
    int j = 0;
	for (int i = 0; i < MAX_TASKS; i++) {
		if (tasks[i].active) {
            j++;
        }
    }
    return j;
#endif
}