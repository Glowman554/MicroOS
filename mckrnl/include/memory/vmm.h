#pragma once

#include <stdint.h>

#define PTE_PRESENT 0x1
#define PTE_WRITE   0x2
#define PTE_USER	0x4
#define PTE_PWT	 0x8

#define USER_SPACE_OFFSET 0xC0000000

#define ALIGN_PAGE_DOWN(x) ((x) & ~0xFFF)

#define MB (1024 * 1024)

typedef struct vmm_context {
	uint32_t* pagedir;
} vmm_context_t;

extern vmm_context_t* kernel_context;

void vmm_init(void);

vmm_context_t* vmm_create_context(void);
vmm_context_t vmm_get_current_context(void);

int vmm_map_page(vmm_context_t* context, uintptr_t virt, uintptr_t phys, uint32_t flags);
void vmm_clone_kernel_context(vmm_context_t* context);
void vmm_destroy_context(vmm_context_t* context);

void vmm_activate_context(vmm_context_t* context);

void vmm_synchronize_task_contexts_with_kernel_context();

void* vmm_alloc(uint32_t num_pages);
void* vmm_calloc(uint32_t num_pages);
void vmm_free(void* ptr, uint32_t num_pages);

#define PAGES_OF(type) (sizeof(type) / 0x1000 + 1)
#define TO_PAGES(num) ((num) / 0x1000 + 1)
void* vmm_resize(int data_size, int old_size, int new_size, void* ptr);

void vmm_read_context(void* ptr, void* out, int sz, vmm_context_t* context);
void vmm_write_context(void* ptr, void* in, int sz, vmm_context_t* context);
void* vmm_lookup(uintptr_t ptr, vmm_context_t* context);