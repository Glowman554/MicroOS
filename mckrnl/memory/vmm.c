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

    printf("eip: %x\n", registers->eip);

	halt();
	return registers;
}

void vmm_identity_map(uintptr_t start, uintptr_t end) {
    while (start < end) {
		vmm_map_page(kernel_context, start, start, PTE_PRESENT | PTE_WRITE);
        start += 0x1000;
    }
}

extern const void kernel_start;
extern const void kernel_end;

extern const void paging_start;
extern const void paging_end;


void vmm_init(void) {
	debugf("Initializing virtual memory manager");

	kernel_context = vmm_create_context();

    // for (int i = 0; i < 32 * MB; i += 0x1000) {
	// 	vmm_map_page(kernel_context, i, i, PTE_PRESENT | PTE_WRITE);
	// }

    vmm_identity_map((uintptr_t) &kernel_start, (uintptr_t) &kernel_end);
    vmm_identity_map((uintptr_t) &paging_start, (uintptr_t) &paging_end);

    debugf("Mapping framebuffer...");
#ifdef TEXT_MODE_EMULATION
    for (int i = 0; i < global_multiboot_info->fb_height * (global_multiboot_info->fb_pitch / 4) * (global_multiboot_info->fb_bpp / 8); i += 0x1000) {
	#ifdef RAW_FRAMEBUFFER_ACCESS
		vmm_map_page(kernel_context, global_multiboot_info->fb_addr + i, global_multiboot_info->fb_addr + i, PTE_PRESENT | PTE_WRITE | PTE_USER);
	#else
		vmm_map_page(kernel_context, global_multiboot_info->fb_addr + i, global_multiboot_info->fb_addr + i, PTE_PRESENT | PTE_WRITE);
	#endif
        pmm_mark_used((void*) (uint32_t) global_multiboot_info->fb_addr + i);
    }
#else
    uintptr_t fb_start = 0xb8000;
    uintptr_t fb_end = fb_start + 80 * 25 * 2;
    while (fb_start < fb_end) {
        vmm_map_page(kernel_context, fb_start, fb_start, PTE_PRESENT | PTE_WRITE);
        pmm_mark_used((void*) fb_start);
        fb_start += 0x1000;
    }
#endif

    debugf("Mapping multiboot structure...");
	struct multiboot_module* modules = global_multiboot_info->mbs_mods_addr;
    vmm_map_page(kernel_context, (uintptr_t) global_multiboot_info, (uintptr_t) global_multiboot_info, PTE_PRESENT);
    vmm_map_page(kernel_context, (uintptr_t) modules, (uintptr_t) modules, PTE_PRESENT);

	int i;
	for (i = 0; i < global_multiboot_info->mbs_mods_count; i++) {
		uint32_t addr = modules[i].mod_start;
		while (addr < modules[i].mod_end) {
            vmm_map_page(kernel_context, addr, addr, PTE_PRESENT);
			addr += 0x1000;
		}
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
	struct vmm_context* context = pmm_alloc_pagetable();

	context->pagedir = pmm_alloc_pagetable();

	for (int i = 0; i < 1024; i++) {
		context->pagedir[i] = 0;
	}

	return context;
}

extern task_t tasks[MAX_TASKS];


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
            // if (((uint32_t*) kernel_context->pagedir[pd_index])[pt_index] & PTE_PRESENT) {
                abortf("%x is overlapping with the kernel!", virt);
            // }
		}
    }

	if (context->pagedir[pd_index] & PTE_PRESENT) {
		page_table = (uint32_t*) (context->pagedir[pd_index] & ~0xFFF);
	} else {
		page_table = pmm_alloc_pagetable();
		for (int i = 0; i < 1024; i++) {
			page_table[i] = 0;
		}
		context->pagedir[pd_index] = (uint32_t) page_table | PTE_PRESENT | flags;

        if (context == kernel_context) {
            for (int i = 0; i < MAX_TASKS; i++) {
            	if (tasks[i].active) {
                    tasks[i].context->pagedir[pd_index] = context->pagedir[pd_index];
                }
            }
        }
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
			// if (context->pagedir[i] == kernel_context->pagedir[i]) {
			// 	continue;
			// } else if (kernel_context->pagedir[i] & PTE_PRESENT) {
			// 	abortf("Possible unstable state!");
			// }

			uint32_t* page_table = (uint32_t*) (context->pagedir[i] & ~0xFFF);
			for (int j = 0; j < 1024; j++) {
				if (page_table[j] & PTE_PRESENT) {
					if (kernel_context->pagedir[i] & PTE_PRESENT) {
						uint32_t* kernel_page_table = (uint32_t*) (kernel_context->pagedir[i] & ~0xFFF);
						if (kernel_page_table[j] & PTE_PRESENT) {
							continue;
						}
					}

					// void* virt = (void*) ((i << 22) | (j << 12));
					void* phys = (void*) (page_table[j] & ~0xFFF);

					// debugf("Unmapping %p / %p", virt, phys);
					pmm_free(phys);
				}
			}

			if (!(kernel_context->pagedir[i] & PTE_PRESENT)) {
				pmm_free(page_table);
			}
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

void* vmm_calloc(uint32_t num_pages) {
	void* ptr = vmm_alloc(num_pages);
	memset(ptr, 0, num_pages * 0x1000);
	return ptr;
}

void vmm_free(void* ptr, uint32_t num_pages) {
	pmm_free_range(ptr, num_pages);
}

void* vmm_resize(int data_size, int old_size, int new_size, void* ptr) {
	int new_size_p = (data_size * new_size) / 0x1000 + 1;
	int old_size_p = (data_size * old_size) / 0x1000 + 1;

	if (ptr == NULL) {
		return vmm_alloc(new_size_p);
	}

	if (new_size == 0) {
		debugf("Deallocating...");
		vmm_free(ptr, old_size_p);
		return NULL;
	}

	if (new_size_p != old_size_p) {
		// debugf("resizing %d pages to %d pages!", old_size_p, new_size_p);
		void* new_ptr = vmm_alloc(new_size_p);
		if (new_size_p < old_size_p) {
			memcpy(new_ptr, ptr, data_size * new_size);
		} else {
			memcpy(new_ptr, ptr, data_size * old_size);
		}
		
		vmm_free(ptr, old_size_p);

		return new_ptr;
	}

	return ptr;
}

void vmm_read_context(void* ptr, void* out, int sz, vmm_context_t* context) {
	static uint8_t buffer[0x1000] = { 0 };
	assert(sz <= sizeof(buffer));

	vmm_context_t current = vmm_get_current_context();

	vmm_activate_context(context);
	memcpy(buffer, ptr, sz);

	vmm_activate_context(&current);
	memcpy(out, buffer, sz);
}

void* vmm_lookup(uintptr_t ptr, vmm_context_t* context) {
	uint32_t page_index = ptr / 0x1000;

	uint32_t pd_index = page_index / 1024;
	uint32_t pt_index = page_index % 1024;

	if (context->pagedir[pd_index] & PTE_PRESENT) {
		uint32_t* page_table = (uint32_t*) (context->pagedir[pd_index] & ~0xFFF);
		if (page_table[pt_index] & PTE_PRESENT) {
			void* phys = (void*) (page_table[pt_index] & ~0xFFF) + (ptr % 0x1000);
			return phys;
		}
	}

	return NULL;
}