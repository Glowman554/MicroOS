#include <memory/vmm.h>

#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <stddef.h>

vmm_context_t* kernel_context;

char* page_fault_get_error(uint32_t error) {
	switch (error) {
		case 0x00:
			return "Supervisory process tried to read a non-present page entry";
		case 0x01:
			return "Supervisory process tried to read a page and caused a protection fault";
		case 0x02:
			return "Supervisory process tried to write to a non-present page entry";
		case 0x03:
			return "Supervisory process tried to write a page and caused a protection fault";
		case 0x04:
			return "User process tried to read a non-present page entry";
		case 0x05:
			return "User process tried to read a page and caused a protection fault";
		case 0x06:
			return "User process tried to write to a non-present page entry";
		case 0x07:
			return "User process tried to write a page and caused a protection fault";
		default:
			return "Unknown error";
	}
}

cpu_registers_t* page_fault_handler(cpu_registers_t* registers, void* _) {
	printf("Page fault!\n");
	printf("Error: %s\n", page_fault_get_error(registers->error));
	
	uint32_t cr2;
	asm volatile("mov %%cr2, %0" : "=r" (cr2));

	printf("Faulting address: %x\n", cr2);

	halt();
}

void vmm_init(void) {
	debugf("Initializing virtual memory manager");

	kernel_context = vmm_create_context();
	for (int i = 0; i < 32 * MB; i += 0x1000) {
		vmm_map_page(kernel_context, i, i, PTE_PRESENT | PTE_WRITE);
	}

	vmm_map_page(kernel_context, (uintptr_t) NULL, (uintptr_t) NULL, 0);

	vmm_activate_context(kernel_context);

	register_interrupt_handler(0xe, page_fault_handler, NULL);

	debugf("Activating paging NOW!");
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

}

vmm_context_t* vmm_create_context(void) {
	struct vmm_context* context = pmm_alloc();

	context->pagedir = pmm_alloc();

	for (int i = 0; i < 1024; i++) {
		context->pagedir[i] = 0;
	}

    return context;
}

int vmm_map_page(vmm_context_t* context, uintptr_t virt, uintptr_t phys, uint32_t flags) {
	// debugf("Mapping page %x to %x (flags: %x)", virt, phys, flags);
	uint32_t page_index = virt / 0x1000;
	uint32_t pd_index = page_index / 1024;
	uint32_t pt_index = page_index % 1024;

	uint32_t* page_table;

	if ((virt & 0xFFF) || (phys & 0xFFF)) {
		return -1;
	}

	if (context != kernel_context) {
		if (kernel_context->pagedir[pd_index] & PTE_PRESENT) {
			abortf("%x is overlapping with the kernel!", virt);
		}
	}

	if (context->pagedir[pd_index] & PTE_PRESENT) {
		page_table = (uint32_t*) (context->pagedir[pd_index] & ~0xFFF);
	} else {
		page_table = pmm_alloc();
		for (int i = 0; i < 1024; i++) {
			page_table[i] = 0;
		}
		context->pagedir[pd_index] = (uint32_t) page_table | PTE_PRESENT | flags;
	}

	page_table[pt_index] = phys | flags;
	asm volatile("invlpg %0" : : "m" (*(char*)virt));

	return 0;

}

void vmm_clone_kernel_context(vmm_context_t* context) {
	// debugf("Mapping kernel into context %p", context);

	for (int i = 0; i < 1024; i++) {
		if (kernel_context->pagedir[i] & PTE_PRESENT) {
			// uint32_t* page_table = (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF);
			// for (int j = 0; j < 1024; j++) {
			// 	if (page_table[j] & PTE_PRESENT) {
			// 		void* virt = (void*) ((i << 22) | (j << 12));
			// 		void* phys = (void*) (page_table[j] & ~0xFFF);

			// 		vmm_map_page(context, (uintptr_t) virt, (uintptr_t) phys, page_table[j] & 0xFFF);
			// 	}
			// }

			context->pagedir[i] = kernel_context->pagedir[i];
		}
	}
}

void vmm_destroy_context(vmm_context_t* context) {
	// debugf("Destroying context %p", context);

	// for (int i = 0; i < 1024; i++) {
	// 	if (kernel_context->pagedir[i] & PTE_PRESENT) {
	// 		uint32_t* page_table = (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF);
	// 		for (int j = 0; j < 1024; j++) {
	// 			if (page_table[j] & PTE_PRESENT) {
	// 				void* virt = (void*) ((i << 22) | (j << 12));
	// 				void* phys = (void*) (page_table[j] & ~0xFFF);

	// 				vmm_map_page(context, (uintptr_t) virt, (uintptr_t) phys, 0);
	// 			}
	// 		}
	// 	}
	// }

	for (int i = 0; i < 1024; i++) {
		if (context->pagedir[i] & PTE_PRESENT) {
			if (context->pagedir[i] == kernel_context->pagedir[i]) {
				continue;
			}

			uint32_t* page_table = (uint32_t*) (context->pagedir[i] & ~0xFFF);
			for (int j = 0; j < 1024; j++) {
				if (page_table[j] & PTE_PRESENT) {
					void* virt = (void*) ((i << 22) | (j << 12));
					void* phys = (void*) (page_table[j] & ~0xFFF);

					debugf("Unmapping %p / %p", virt, phys);
					pmm_free(phys);
				}
			}

			pmm_free(page_table);
		}
	}

	// for (int i = 0; i < 1024; i++) {
	// 	if (context->pagedir[i] & PTE_PRESENT) {
	// 		uint32_t* page_table = (uint32_t*) (context->pagedir[i] & ~0xFFF);
	// 		pmm_free(page_table);
	// 	}
	// }

	pmm_free(context->pagedir);
	pmm_free(context);
}

void vmm_synchronize_task_contexts_with_kernel_context(void) {
	// debugf("Synchronizing task contexts with kernel context");

	// for (int i = 0; i < MAX_TASKS; i++) {
	// 	if (tasks[i].active) {
	// 		vmm_clone_kernel_context(tasks[i].context);
	// 	}
	// }
}

void vmm_activate_context(vmm_context_t* context) {
	// debugf("Activating context pagedir %p", context->pagedir);
	asm volatile("mov %0, %%cr3" : : "r" (context->pagedir));
}

vmm_context_t vmm_get_current_context(void) {
	vmm_context_t context;
	asm volatile("mov %%cr3, %0" : "=r" (context.pagedir));
	return context;
}

void* vmm_alloc(uint32_t num_pages) {
	void* ptr = pmm_alloc_range(num_pages);

	for (int i = 0; i < num_pages; i++) {
		vmm_map_page(kernel_context, (uintptr_t) ptr + (i * 0x1000), (uintptr_t) ptr + (i * 0x1000), PTE_PRESENT | PTE_WRITE);
	}
	
	vmm_synchronize_task_contexts_with_kernel_context();
	return ptr;
}

void vmm_free(void* ptr, uint32_t num_pages) {
	pmm_free_range(ptr, num_pages);
}
