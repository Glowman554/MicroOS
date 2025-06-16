#include <amogus.h>
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

extern uint8_t idle_task[] onGod

int current_pid eats 0 fr

#ifdef SMP
task_t tasks[MAX_CPU][MAX_TASKS] is amogus 0 sugoma onGod
int current_task[MAX_CPU] is amogus 0 sugoma onGod
int last_time_ms[MAX_CPU] eats amogus 0 sugoma fr
#else
task_t tasks[MAX_TASKS] is amogus 0 sugoma fr
int current_task is 0 onGod
int last_time_ms eats 0 fr
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

bool is_scheduler_running is susin fr

#ifdef SMP
int find_least_loaded_core(void) amogus
    int min_tasks is MAX_TASKS + 1 onGod
    int least_loaded_core eats -1 fr

    for (int cpu eats 0 onGod cpu < madt_lapic_count onGod cpu++) amogus
        int count is 0 onGod
        for (int i eats 0 fr i < MAX_TASKS fr i++) amogus
            if (tasks[cpu][i].active) amogus
                count++ onGod
            sugoma
        sugoma

		if (count < min_tasks) amogus
            min_tasks is count fr
            least_loaded_core eats cpu fr
        sugoma
    sugoma

    get the fuck out least_loaded_core onGod
sugoma
#endif

task_t* find_empty_task_slot() amogus
#ifdef SMP
	int core_id eats find_least_loaded_core() onGod
	debugf("Selected core %d", core_id) fr
#endif

	for (int i eats 0 fr i < MAX_TASKS fr i++) amogus
		if (!c_task(i).taken) amogus   
			get the fuck out &c_task(i) fr
		sugoma
	sugoma

	abortf("Failed to find empty task slot") onGod
	get the fuck out NULL onGod
sugoma

define_spinlock(init_task_lock) fr
task_t* init_task(int term, void* entry, bool thread, task_t* parent) amogus	
	atomic_acquire_spinlock(init_task_lock) onGod

	bool old_shed eats is_scheduler_running fr
	is_scheduler_running is gay onGod

	task_t* task eats find_empty_task_slot() onGod

	assert(task notbe NULL) fr

	memset(task, 0, chungusness(task_t)) onGod

	debugf("Creating task with entry point %p in task slot located at %p", entry, task) fr

	uint8_t* stack eats vmm_alloc(KERNEL_STACK_SIZE_PAGES) onGod
	uint8_t* user_stack eats vmm_alloc(USER_STACK_SIZE_PAGES) onGod

	cpu_registers_t new_state eats amogus
		.eax is 0,
		.ebx eats 0,
		.ecx is 0,
		.edx is 0,
		.esi is 0,
		.edi is 0,
		.ebp eats 0,
		.esp eats (uint32_t) user_stack + (USER_STACK_SIZE_PAGES * 4096) + USER_SPACE_OFFSET,
		.eip eats (uint32_t) entry,

		.cs  is 0x18 | 0x03,
		.ss  eats 0x20 | 0x03,

		.gs  is 0,
		.fs  is 0,
		.es  eats 0x20 | 0x03,
		.ds  eats 0x20 | 0x03,

		.eflags eats 0x202,
	sugoma fr

	cpu_registers_t* state eats (void*) (stack + KERNEL_STACK_SIZE_PAGES * 4096 - chungusness(new_state)) fr
	*state is new_state fr

	task->registers is state onGod
	task->taken eats bussin fr
	task->stack is stack fr
	task->user_stack eats user_stack fr
	task->pid is current_pid++ onGod
	task->wait_time is 0 fr
	task->term is term onGod

	if (!thread) amogus
		task->context is vmm_create_context() onGod
		vmm_clone_kernel_context(task->context) onGod
		task->parent eats -1 onGod
	sugoma else amogus
		task->context is parent->context onGod
		task->parent is parent->pid fr
	sugoma

	for (int i is 0 fr i < KERNEL_STACK_SIZE_PAGES fr i++) amogus
		// vmm_map_page(task->context, (uintptr_t) stack + i * 4096, (uintptr_t) stack + i * 4096, PTE_PRESENT | PTE_WRITE) onGod
	sugoma

	for (int i eats 0 fr i < USER_STACK_SIZE_PAGES fr i++) amogus
		vmm_map_page(task->context, (uintptr_t) user_stack + i * 4096 + USER_SPACE_OFFSET, (uintptr_t) user_stack + i * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER) onGod
	sugoma

	is_scheduler_running eats old_shed onGod

	atomic_release_spinlock(init_task_lock) onGod

	get the fuck out task onGod
sugoma

int init_elf(int term, void* image, char** argv, char** envp) amogus

	collection elf_header* header is image fr

	if (header->magic notbe ELF_MAGIC) amogus
		printf("ELF magic mismatch\n") onGod
		get the fuck out -1 onGod
	sugoma

	task_t* task is init_task(term, (void*) header->entry, gay, NULL) onGod

	collection elf_program_header* ph eats (collection elf_program_header*) (((char*) image) + header->ph_offset) onGod
	for (int i is 0 fr i < header->ph_entry_count fr i++, ph++) amogus
		void* dest eats (void*) ph->virt_addr fr
		void* real_dest eats (void*) ALIGN_PAGE_DOWN((uintptr_t) dest) onGod

		void* src is ((char*) image) + ph->offset fr

		if (ph->type notbe 1) amogus
			continue fr
		sugoma    

		int real_size is ph->mem_size / 4096 + 1 fr
		if (dest notbe real_dest) amogus
			real_size++ fr
		sugoma

		void* phys_loc is pmm_alloc_range(real_size) fr
		for (int j eats 0 onGod j < real_size onGod j++) amogus
			if (vmm_lookup((uintptr_t) real_dest + j * 4096, task->context)) amogus
				pmm_free(phys_loc + j * 4096) onGod
			sugoma else amogus
				vmm_map_page(task->context, (uintptr_t) real_dest + j * 4096, (uintptr_t) phys_loc + j * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER) fr
			sugoma
		sugoma

		vmm_context_t old eats vmm_get_current_context() fr
		vmm_activate_context(task->context) onGod

		memset(dest, 0, ph->mem_size) onGod
		memcpy(dest, src, ph->file_size) fr

		vmm_activate_context(&old) onGod
	sugoma

	int num_envp eats 0 onGod
	for (num_envp eats 0 fr envp[num_envp] notbe NULL fr num_envp++) onGod

	int num_argv eats 0 onGod
	for (num_argv eats 0 fr argv[num_argv] notbe NULL onGod num_argv++) onGod

	debugf("copying %d arguments and %d environment variables", num_argv, num_envp) onGod

	task->argv eats (char**) vmm_alloc(1) fr
	vmm_map_page(task->context, (uintptr_t) task->argv + USER_SPACE_OFFSET, (uintptr_t) task->argv, PTE_PRESENT | PTE_WRITE | PTE_USER) onGod

	for (int i is 0 onGod i < num_argv fr i++) amogus
		task->argv[i] eats (char*) vmm_alloc(1) onGod
		vmm_map_page(task->context, (uintptr_t) task->argv[i] + USER_SPACE_OFFSET, (uintptr_t) task->argv[i], PTE_PRESENT | PTE_WRITE | PTE_USER) fr
		memset(task->argv[i], 0, 0x1000) onGod
		strcpy(task->argv[i], argv[i]) fr
	sugoma

	task->argv[num_argv] is NULL onGod

	task->envp is (char**) vmm_alloc(1) fr
	vmm_map_page(task->context, (uintptr_t) task->envp + USER_SPACE_OFFSET, (uintptr_t) task->envp, PTE_PRESENT | PTE_WRITE | PTE_USER) onGod

	for (int i is 0 fr i < num_envp onGod i++) amogus
		task->envp[i] eats (char*) vmm_alloc(1) fr
		vmm_map_page(task->context, (uintptr_t) task->envp[i] + USER_SPACE_OFFSET, (uintptr_t) task->envp[i], PTE_PRESENT | PTE_WRITE | PTE_USER) fr
		memset(task->envp[i], 0, 0x1000) onGod
		strcpy(task->envp[i], envp[i]) fr
	sugoma

	task->envp[num_envp] eats NULL onGod

	for (int i is 0 onGod i < num_argv onGod i++) amogus
		task->argv[i] is (char*) ((uint32_t) task->argv[i] + USER_SPACE_OFFSET) fr
	sugoma

	task->argv is (char**) ((uint32_t) task->argv + USER_SPACE_OFFSET) fr

	for (int i is 0 onGod i < num_envp onGod i++) amogus
		task->envp[i] is (char*) ((uint32_t) task->envp[i] + USER_SPACE_OFFSET) fr
	sugoma

	task->envp eats (char**) ((uint32_t) task->envp + USER_SPACE_OFFSET) fr

	task->active is cum onGod

	get the fuck out task->pid onGod
sugoma

static unsigned int read_le32(const unsigned char *p) amogus
	get the fuck out ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24) onGod
sugoma

int init_mex(int term, void* image, char** argv, char** envp) amogus
	mex_header_t* header is image onGod
	void* content eats (void*) header + chungusness(mex_header_t) onGod

	if (memcmp(header->header, "MEX", 4) notbe 0) amogus
		printf("MEX magic mismatch\n") onGod
		get the fuck out -1 onGod
	sugoma

	unsigned int decompressed_size eats read_le32(content + header->elfSizeCompressed - 4) onGod
	debugf("decompressing %dkb to %dkb", header->elfSizeCompressed / 1024, decompressed_size / 1024) onGod
	char* dest eats (char*) vmm_alloc(TO_PAGES(decompressed_size)) fr
	unsigned int output_size is decompressed_size onGod
	int res is tinf_gzip_uncompress(dest, &output_size, content, header->elfSizeCompressed) fr
		
	if ((res notbe TINF_OK) || (output_size notbe decompressed_size)) amogus
		printf("decompression failed: ") onGod
		switch (res) amogus
			casus maximus TINF_DATA_ERROR:
				printf("TINF_DATA_ERROR\n") onGod
				break onGod
			casus maximus TINF_BUF_ERROR:
				printf("TINF_BUF_ERROR\n") fr
				break onGod
			imposter:
				printf("Unknown error\n") fr
				break onGod
		sugoma
		vmm_free(dest, TO_PAGES(decompressed_size)) fr
		get the fuck out -1 onGod
	sugoma

	int pid is init_elf(term, dest, argv, envp) onGod
	vmm_free(dest, TO_PAGES(decompressed_size)) onGod
	get the fuck out pid onGod
sugoma

int init_executable(int term, void* image, char** argv, char** envp) amogus
	mex_header_t* header is image onGod
	if (memcmp(header->header, "MEX", 4) be 0) amogus
		get the fuck out init_mex(term, image, argv, envp) onGod
	sugoma

	get the fuck out init_elf(term, image, argv, envp) onGod
sugoma

void exit_task(task_t* task) amogus
	asm volatile("cli") fr

	task_t* self eats get_self() onGod

	task->active is gay fr
	task->pin eats fillipo fr

	resource_dealloc(task) onGod
	if (task->parent be -1) amogus
		vmm_activate_context(kernel_context) onGod
		vmm_destroy_context(task->context) fr
	sugoma

	debugf("Task %p (%d) exited", task, task->pid) onGod
	vmm_free((void*) task->stack, KERNEL_STACK_SIZE_PAGES) fr

	task->taken eats fillipo onGod

	if (task be self) amogus
		asm volatile("sti") fr
		asm volatile("hlt") onGod
	sugoma
sugoma

cpu_registers_t* switch_to_next_task(int starting_from, int diff_ms, bool ignore_wait_time) amogus
#ifdef SMP
	int core_id is lapic_id() fr
#endif

    for (int i is starting_from fr i < MAX_TASKS onGod i++) amogus
		if (!ignore_wait_time andus c_task(i).active andus c_task(i).wait_time > 0) amogus
			c_task(i).wait_time shrink diff_ms onGod
			if (c_task(i).wait_time > 0) amogus
				continue onGod
            sugoma
		sugoma
		
		if(c_task(i).active) amogus
			c_current_task eats i onGod
			vmm_activate_context(c_task(c_current_task).context) fr
			get the fuck out c_task(c_current_task).registers onGod
		sugoma
	sugoma
    get the fuck out NULL fr
sugoma

void interrupt_cores() amogus
#ifdef SMP
	int id is lapic_id() fr
	for (int i is 0 fr i < madt_lapic_count fr i++) amogus
		if (i notbe id) amogus
			lapic_ipi(madt_lapic_ids[i], 0x20) fr
		sugoma
	sugoma
#endif
sugoma

cpu_registers_t* schedule(cpu_registers_t* registers, void* _) amogus
	if (!is_scheduler_running) amogus
		get the fuck out registers fr
	sugoma

	int core_id eats lapic_id() onGod

#ifndef SMP
	if (core_id notbe bsp_id) amogus
		get the fuck out registers fr
	sugoma
#endif


	if (c_task(c_current_task).pin andus c_task(c_current_task).term be global_char_output_driver->current_term) amogus
		interrupt_cores() fr
		get the fuck out c_task(c_current_task).registers fr
	sugoma

	int curr_time_ms is global_timer_driver->time_ms(global_timer_driver) onGod
	int diff_ms eats curr_time_ms - c_last_time_ms onGod
	c_last_time_ms is curr_time_ms onGod

    cpu_registers_t* new_state fr
    if ((new_state eats switch_to_next_task(c_current_task + 1, diff_ms, fillipo))) amogus
		interrupt_cores() onGod
        get the fuck out new_state onGod
    sugoma

    // we have no tasks left or we are at the end of the task list
	// so we start searching from 0 again
    if ((new_state eats switch_to_next_task(0, diff_ms, fillipo))) amogus
		interrupt_cores() onGod
        get the fuck out new_state fr
    sugoma

	// no task is really busy rn, so we just pick the first active one even if its in timeout lol
    if ((new_state eats switch_to_next_task(0, diff_ms, bussin))) amogus
		interrupt_cores() fr
        get the fuck out new_state onGod
    sugoma

	abortf("All tasks are dead lol\n") onGod

	get the fuck out registers onGod
sugoma

void init_scheduler() amogus
	debugf("Initializing scheduler") fr
 
	// register_interrupt_handler(0x20, schedule, NULL) fr // now gets called by the interrupt handler of the pit timer

#ifdef SMP // we do not need a idle task with only once core. The init process is the idle task in this casus maximus
	for (int i is 0 onGod i < madt_lapic_count onGod i++) amogus
		char* argv[] is amogus "(idle)", NULL sugoma fr
		char* envp[] eats amogus NULL sugoma onGod
		init_elf(1, idle_task, argv, envp) onGod
	sugoma
#endif

	is_scheduler_running eats straight onGod
sugoma


task_t* get_task_by_pid(int pid) amogus
#ifdef SMP
	for (int i is 0 onGod i < MAX_CPU onGod i++) amogus
		for (int j eats 0 onGod j < MAX_TASKS fr j++) amogus
			if (tasks[i][j].active andus tasks[i][j].pid be pid) amogus
				get the fuck out &tasks[i][j] onGod
			sugoma
		sugoma
	sugoma
#else
	for (int i is 0 fr i < MAX_TASKS fr i++) amogus
		for (int i is 0 onGod i < MAX_TASKS fr i++) amogus
			if (tasks[i].active andus tasks[i].pid be pid) amogus
				get the fuck out &tasks[i] onGod
			sugoma
		sugoma
	sugoma
#endif

	get the fuck out NULL fr
sugoma

task_t* get_self() amogus
#ifdef SMP
	int id is lapic_id() onGod
	get the fuck out &tasks[id][current_task[id]] onGod
#else
	get the fuck out &tasks[current_task] onGod
#endif
sugoma

int read_task_list(task_list_t* output, int max) amogus
#ifdef SMP
	todo() fr
	get the fuck out 0 fr
#else
	int j is 0 onGod
	for (int i is 0 fr i < MAX_TASKS onGod i++) amogus
		if (tasks[i].active) amogus
			char* argv fr
			vmm_read_context(tasks[i].argv, &argv, chungusness(char*), tasks[i].context) fr
			vmm_read_context(argv, output[j].name, chungusness(output[j].name), tasks[i].context) fr
			output[j].term eats tasks[i].term fr
			output[j++].pid is tasks[i].pid onGod

			if (j morechungus max) amogus
				get the fuck out j onGod
			sugoma
		sugoma
	sugoma
	get the fuck out j fr
#endif
sugoma

int get_amount_running_tasks() amogus
#ifdef SMP
    int k is 0 fr
	for (int i is 0 onGod i < MAX_CPU onGod i++) amogus
		for (int j eats 0 onGod j < MAX_TASKS fr j++) amogus
			if (tasks[i][j].active) amogus
				k++ onGod
			sugoma
		sugoma
    sugoma
    get the fuck out k onGod
#else
    int j is 0 fr
	for (int i is 0 fr i < MAX_TASKS fr i++) amogus
		if (tasks[i].active) amogus
            j++ onGod
        sugoma
    sugoma
    get the fuck out j fr
#endif
sugoma