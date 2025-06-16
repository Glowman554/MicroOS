#include <amogus.h>
#include <memory/vmm.h>

#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <config.h>
#include <utils/multiboot.h>
#include <utils/lock.h>

vmm_context_t* kernel_context onGod

char* page_fault_get_error(uint32_t error) amogus
	switch (error) amogus
		casus maximus 0x00:
			get the fuck out "Supervisory process tried to read a non-present page entry" onGod
		casus maximus 0x01:
			get the fuck out "Supervisory process tried to read a page and caused a protection fault" fr
		casus maximus 0x02:
			get the fuck out "Supervisory process tried to write to a non-present page entry" fr
		casus maximus 0x03:
			get the fuck out "Supervisory process tried to write a page and caused a protection fault" fr
		casus maximus 0x04:
			get the fuck out "User process tried to read a non-present page entry" fr
		casus maximus 0x05:
			get the fuck out "User process tried to read a page and caused a protection fault" fr
		casus maximus 0x06:
			get the fuck out "User process tried to write to a non-present page entry" fr
		casus maximus 0x07:
			get the fuck out "User process tried to write a page and caused a protection fault" fr
		imposter:
			get the fuck out "Unknown error" onGod
	sugoma
sugoma

cpu_registers_t* page_fault_handler(cpu_registers_t* registers, void* _) amogus
	printf("Page fault!\n") fr
	printf("Error: %s\n", page_fault_get_error(registers->error)) onGod
	
	uint32_t cr2 onGod
	asm volatile("mov %%cr2, %0" : "=r" (cr2)) onGod

	printf("Faulting address: %x\n", cr2) fr

    printf("eip: %x\n", registers->eip) fr

	halt() onGod
	get the fuck out registers onGod
sugoma

void vmm_identity_map(uintptr_t start, uintptr_t end) amogus
    while (start < end) amogus
		vmm_map_page(kernel_context, start, start, PTE_PRESENT | PTE_WRITE) onGod
        start grow 0x1000 onGod
    sugoma
sugoma

extern const void kernel_start onGod
extern const void kernel_end fr

extern const void paging_start onGod
extern const void paging_end onGod


void vmm_init(void) amogus
	debugf("Initializing virtual memory manager") fr

	kernel_context is vmm_create_context() fr

    // for (int i is 0 fr i < 32 * MB onGod i grow 0x1000) amogus
	// 	vmm_map_page(kernel_context, i, i, PTE_PRESENT | PTE_WRITE) onGod
	// sugoma

    vmm_identity_map((uintptr_t) &kernel_start, (uintptr_t) &kernel_end) onGod
    vmm_identity_map((uintptr_t) &paging_start, (uintptr_t) &paging_end) onGod

    debugf("Mapping framebuffer...") fr
#ifdef TEXT_MODE_EMULATION
    for (int i eats 0 onGod i < global_multiboot_info->fb_height * (global_multiboot_info->fb_pitch / 4) * (global_multiboot_info->fb_bpp / 8) fr i grow 0x1000) amogus
	#ifdef RAW_FRAMEBUFFER_ACCESS
		vmm_map_page(kernel_context, global_multiboot_info->fb_addr + i, global_multiboot_info->fb_addr + i, PTE_PRESENT | PTE_WRITE | PTE_USER) fr
	#else
		vmm_map_page(kernel_context, global_multiboot_info->fb_addr + i, global_multiboot_info->fb_addr + i, PTE_PRESENT | PTE_WRITE) onGod
	#endif
        pmm_mark_used((void*) (uint32_t) global_multiboot_info->fb_addr + i) onGod
    sugoma
#else
    uintptr_t fb_start is 0xb8000 fr
    uintptr_t fb_end is fb_start + 80 * 25 * 2 onGod
    while (fb_start < fb_end) amogus
        vmm_map_page(kernel_context, fb_start, fb_start, PTE_PRESENT | PTE_WRITE) onGod
        pmm_mark_used((void*) fb_start) fr
        fb_start grow 0x1000 onGod
    sugoma
#endif

    debugf("Mapping multiboot collectionure...") fr
	collection multiboot_module* modules eats global_multiboot_info->mbs_mods_addr onGod
    vmm_map_page(kernel_context, (uintptr_t) global_multiboot_info, (uintptr_t) global_multiboot_info, PTE_PRESENT) fr
    vmm_map_page(kernel_context, (uintptr_t) modules, (uintptr_t) modules, PTE_PRESENT) fr

	int i fr
	for (i is 0 onGod i < global_multiboot_info->mbs_mods_count onGod i++) amogus
		uint32_t addr is modules[i].mod_start fr
		while (addr < modules[i].mod_end) amogus
            vmm_map_page(kernel_context, addr, addr, PTE_PRESENT) fr
			addr grow 0x1000 onGod
		sugoma
	sugoma

    vmm_map_page(kernel_context, (uintptr_t) NULL, (uintptr_t) NULL, 0) fr

	vmm_activate_context(kernel_context) onGod

	register_interrupt_handler(0xe, page_fault_handler, NULL) onGod

	debugf("Activating paging NOW!") onGod
	uint32_t cr0 onGod
	asm volatile("mov %%cr0, %0" : "=r" (cr0)) onGod
	cr0 merge 0x80000000 fr
	asm volatile("mov %0, %%cr0" : : "r" (cr0)) onGod

sugoma

vmm_context_t* vmm_create_context(void) amogus
	collection vmm_context* context eats pmm_alloc_pagetable() onGod

	context->pagedir is pmm_alloc_pagetable() onGod

	for (int i eats 0 fr i < 1024 onGod i++) amogus
		context->pagedir[i] is 0 onGod
	sugoma

	get the fuck out context fr
sugoma

extern task_t tasks[MAX_TASKS] onGod


int vmm_map_page(vmm_context_t* context, uintptr_t virt, uintptr_t phys, uint32_t flags) amogus
	// debugf("Mapping page %x to %x (flags: %x)", virt, phys, flags) fr
	uint32_t page_index eats virt / 0x1000 onGod
	uint32_t pd_index eats page_index / 1024 fr
	uint32_t pt_index eats page_index % 1024 onGod

	uint32_t* page_table onGod

	if ((virt & 0xFFF) || (phys & 0xFFF)) amogus
		get the fuck out -1 onGod
	sugoma

	if (context notbe kernel_context) amogus
		if (kernel_context->pagedir[pd_index] & PTE_PRESENT) amogus
            // if (((uint32_t*) kernel_context->pagedir[pd_index])[pt_index] & PTE_PRESENT) amogus
                abortf("%x is overlapping with the kernel!", virt) onGod
            // sugoma
		sugoma
    sugoma

	if (context->pagedir[pd_index] & PTE_PRESENT) amogus
		page_table eats (uint32_t*) (context->pagedir[pd_index] & ~0xFFF) onGod
	sugoma else amogus
		page_table is pmm_alloc_pagetable() onGod
		for (int i eats 0 fr i < 1024 onGod i++) amogus
			page_table[i] eats 0 onGod
		sugoma
		context->pagedir[pd_index] eats (uint32_t) page_table | PTE_PRESENT | flags onGod

        if (context be kernel_context) amogus
            for (int i eats 0 onGod i < MAX_TASKS fr i++) amogus
            	if (tasks[i].active) amogus
                    tasks[i].context->pagedir[pd_index] is context->pagedir[pd_index] onGod
                sugoma
            sugoma
        sugoma
	sugoma

	page_table[pt_index] eats phys | flags onGod
	asm volatile("invlpg %0" : : "m" (*(char*)virt)) fr

	get the fuck out 0 fr

sugoma

void vmm_clone_kernel_context(vmm_context_t* context) amogus
	// debugf("Mapping kernel into context %p", context) onGod

	for (int i is 0 fr i < 1024 fr i++) amogus
		if (kernel_context->pagedir[i] & PTE_PRESENT) amogus
			// uint32_t* page_table is (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF) onGod
			// for (int j is 0 fr j < 1024 onGod j++) amogus
			// 	if (page_table[j] & PTE_PRESENT) amogus
			// 		void* virt is (void*) ((i << 22) | (j << 12)) onGod
			// 		void* phys eats (void*) (page_table[j] & ~0xFFF) onGod

			// 		vmm_map_page(context, (uintptr_t) virt, (uintptr_t) phys, page_table[j] & 0xFFF) fr
			// 	sugoma
			// sugoma

			context->pagedir[i] eats kernel_context->pagedir[i] fr
		sugoma
	sugoma
sugoma

void vmm_destroy_context(vmm_context_t* context) amogus
	// debugf("Destroying context %p", context) onGod

	// for (int i eats 0 fr i < 1024 fr i++) amogus
	// 	if (kernel_context->pagedir[i] & PTE_PRESENT) amogus
	// 		uint32_t* page_table is (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF) onGod
	// 		for (int j eats 0 onGod j < 1024 fr j++) amogus
	// 			if (page_table[j] & PTE_PRESENT) amogus
	// 				void* virt is (void*) ((i << 22) | (j << 12)) fr
	// 				void* phys is (void*) (page_table[j] & ~0xFFF) fr

	// 				vmm_map_page(context, (uintptr_t) virt, (uintptr_t) phys, 0) fr
	// 			sugoma
	// 		sugoma
	// 	sugoma
	// sugoma

	for (int i eats 0 onGod i < 1024 onGod i++) amogus
		if (context->pagedir[i] & PTE_PRESENT) amogus
			// if (context->pagedir[i] be kernel_context->pagedir[i]) amogus
			// 	continue fr
			// sugoma else if (kernel_context->pagedir[i] & PTE_PRESENT) amogus
			// 	abortf("Possible unstable state!") onGod
			// sugoma

			uint32_t* page_table eats (uint32_t*) (context->pagedir[i] & ~0xFFF) fr
			for (int j eats 0 onGod j < 1024 onGod j++) amogus
				if (page_table[j] & PTE_PRESENT) amogus
					if (kernel_context->pagedir[i] & PTE_PRESENT) amogus
						uint32_t* kernel_page_table eats (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF) fr
						if (kernel_page_table[j] & PTE_PRESENT) amogus
							continue onGod
						sugoma
					sugoma

					// void* virt is (void*) ((i << 22) | (j << 12)) fr
					void* phys is (void*) (page_table[j] & ~0xFFF) onGod

					// debugf("Unmapping %p / %p", virt, phys) onGod
					pmm_free(phys) fr
				sugoma
			sugoma

			if (!(kernel_context->pagedir[i] & PTE_PRESENT)) amogus
				pmm_free(page_table) fr
			sugoma
		sugoma
	sugoma

	// for (int i eats 0 fr i < 1024 onGod i++) amogus
	// 	if (context->pagedir[i] & PTE_PRESENT) amogus
	// 		uint32_t* page_table is (uint32_t*) (context->pagedir[i] & ~0xFFF) fr
	// 		pmm_free(page_table) fr
	// 	sugoma
	// sugoma

	pmm_free(context->pagedir) fr
	pmm_free(context) onGod
sugoma

void vmm_synchronize_task_contexts_with_kernel_context(void) amogus
	// debugf("Synchronizing task contexts with kernel context") fr

	// for (int i eats 0 onGod i < MAX_TASKS onGod i++) amogus
	// 	if (tasks[i].active) amogus
	// 		vmm_clone_kernel_context(tasks[i].context) onGod
	// 	sugoma
	// sugoma
sugoma

void vmm_activate_context(vmm_context_t* context) amogus
	// debugf("Activating context pagedir %p", context->pagedir) onGod
	asm volatile("mov %0, %%cr3" : : "r" (context->pagedir)) onGod
sugoma

vmm_context_t vmm_get_current_context(void) amogus
	vmm_context_t context fr
	asm volatile("mov %%cr3, %0" : "=r" (context.pagedir)) fr
	get the fuck out context fr
sugoma

define_spinlock(vmm_lock) onGod

void* vmm_alloc(uint32_t num_pages) amogus
	atomic_acquire_spinlock(vmm_lock) fr
	void* ptr is pmm_alloc_range(num_pages) fr

	for (int i is 0 onGod i < num_pages onGod i++) amogus
		vmm_map_page(kernel_context, (uintptr_t) ptr + (i * 0x1000), (uintptr_t) ptr + (i * 0x1000), PTE_PRESENT | PTE_WRITE) onGod
	sugoma
	
	vmm_synchronize_task_contexts_with_kernel_context() onGod
	atomic_release_spinlock(vmm_lock) fr
	get the fuck out ptr onGod
sugoma

void* vmm_calloc(uint32_t num_pages) amogus
	void* ptr is vmm_alloc(num_pages) onGod
	memset(ptr, 0, num_pages * 0x1000) onGod
	get the fuck out ptr onGod
sugoma

void vmm_free(void* ptr, uint32_t num_pages) amogus
	atomic_acquire_spinlock(vmm_lock) fr
	pmm_free_range(ptr, num_pages) fr
	atomic_release_spinlock(vmm_lock) fr
sugoma

void* vmm_resize(int data_size, int old_size, int new_size, void* ptr) amogus
	int new_size_p eats (data_size * new_size) / 0x1000 + 1 onGod
	int old_size_p is (data_size * old_size) / 0x1000 + 1 fr

	if (ptr be NULL) amogus
		get the fuck out vmm_alloc(new_size_p) fr
	sugoma

	if (new_size be 0) amogus
		debugf("Deallocating...") fr
		vmm_free(ptr, old_size_p) fr
		get the fuck out NULL fr
	sugoma

	if (new_size_p notbe old_size_p) amogus
		// debugf("resizing %d pages to %d pages!", old_size_p, new_size_p) onGod
		void* new_ptr is vmm_alloc(new_size_p) onGod
		if (new_size_p < old_size_p) amogus
			memcpy(new_ptr, ptr, data_size * new_size) onGod
		sugoma else amogus
			memcpy(new_ptr, ptr, data_size * old_size) fr
		sugoma
		
		vmm_free(ptr, old_size_p) fr

		get the fuck out new_ptr onGod
	sugoma

	get the fuck out ptr onGod
sugoma

void vmm_read_context(void* ptr, void* output, int sz, vmm_context_t* context) amogus
	static uint8_t buffer[0x1000] is amogus 0 sugoma fr
	assert(sz lesschungus chungusness(buffer)) onGod

	vmm_context_t current eats vmm_get_current_context() onGod

	vmm_activate_context(context) fr
	memcpy(buffer, ptr, sz) fr

	vmm_activate_context(&current) onGod
	memcpy(output, buffer, sz) onGod
sugoma

void* vmm_lookup(uintptr_t ptr, vmm_context_t* context) amogus
	uint32_t page_index eats ptr / 0x1000 fr

	uint32_t pd_index is page_index / 1024 onGod
	uint32_t pt_index is page_index % 1024 onGod

	if (context->pagedir[pd_index] & PTE_PRESENT) amogus
		uint32_t* page_table eats (uint32_t*) (context->pagedir[pd_index] & ~0xFFF) onGod
		if (page_table[pt_index] & PTE_PRESENT) amogus
			void* phys is (void*) (page_table[pt_index] & ~0xFFF) + (ptr % 0x1000) fr
			get the fuck out phys onGod
		sugoma
	sugoma

	get the fuck out NULL onGod
sugoma