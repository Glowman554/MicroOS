#include <memory/pmm.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <utils/multiboot.h>
#include <utils/string.h>
#include <config.h>

extern const void kernel_start;
extern const void kernel_end;

extern const void paging_start;
extern const void paging_end;

char paging_data[PAGING_DATA_PAGES * 0x1000] __attribute__((section(".paging"))) = { 0 };

static uint32_t bitmap[BITMAP_SIZE];
static uint32_t bitmap_pagetable_mask[BITMAP_SIZE];

void bitmap_set(uint32_t* map, uint32_t bit) {
	map[bit / 32] &= ~(0b10000000000000000000000000000000 >> (bit % 32));
}

void bitmap_clear(uint32_t* map, uint32_t bit) {
	map[bit / 32] |= (0b10000000000000000000000000000000 >> (bit % 32));
}

int bitmap_test(uint32_t* map, uint32_t bit) {
	return !(map[bit / 32] & (0b10000000000000000000000000000000 >> (bit % 32)));
}

uint32_t free_memory;
uint32_t used_memory;

void pmm_debug_print() {
    debugf("--- PMM DEBUG ---");
    debugf("Used: %d kb (%d mb)", used_memory / 1024, used_memory / 1024 / 1024);
    debugf("Free: %d kb (%d mb)", free_memory / 1024, free_memory / 1024 / 1024);
    debugf("Total: %d kb (%d mb)", (free_memory + used_memory) / 1024, (free_memory + used_memory) / 1024 / 1024);
    debugf("-----------------");
}

void pmm_usage_use(uint32_t ammount) {
    free_memory -= ammount * 0x1000;
    used_memory += ammount * 0x1000;
}


void pmm_usage_unuse(uint32_t ammount) {
    free_memory += ammount * 0x1000;
    used_memory -= ammount * 0x1000;
}

void pmm_init() {
	debugf("Initializing physical memory manager");

	multiboot_mmap_t* mmap = global_multiboot_info->mbs_mmap_addr;
	multiboot_mmap_t* mmap_end = (void*) ((uintptr_t) global_multiboot_info->mbs_mmap_addr + global_multiboot_info->mbs_mmap_length);

	memset(bitmap, 0, sizeof(bitmap));
	memset(bitmap_pagetable_mask, 0xff, sizeof(bitmap_pagetable_mask));

	while (mmap < mmap_end) {
		if (mmap->type == 1) {
			uintptr_t addr = mmap->base;
			uintptr_t end_addr = addr + mmap->length;

            if (mmap->base > 0xfffff000) {
                debugf("WARNING: MMAP section out of range");
                mmap++;
                continue;
            }

            if ((mmap->base + mmap->length) > 0xffffffff) {
                debugf("WARNING: MMAP section too big");
                end_addr = 0xfffff000;
            }

			while (addr < end_addr) {
				pmm_free((void*) addr);
				addr += 0x1000;
			}
		}
		mmap++;
	}

    used_memory= 0;

	uintptr_t addr = (uintptr_t) &kernel_start;
	while (addr < (uintptr_t) &kernel_end) {
		pmm_mark_used((void*) addr);
		addr += 0x1000;
	}
  
    addr = (uintptr_t) &paging_start;
    while (addr < (uintptr_t) &paging_end) {
		bitmap_set(bitmap_pagetable_mask, addr / 0x1000);
		addr += 0x1000;
	}

	debugf("Marking multiboot structures as used");

	struct multiboot_module* modules = global_multiboot_info->mbs_mods_addr;

	pmm_mark_used(global_multiboot_info);
	pmm_mark_used(modules);

	int i;
	for (i = 0; i < global_multiboot_info->mbs_mods_count; i++) {
		addr = modules[i].mod_start;
		while (addr < modules[i].mod_end) {
			pmm_mark_used((void*) addr);
			addr += 0x1000;
		}
	}

	pmm_mark_used(NULL);

    pmm_debug_print();
}

void* pmm_alloc() {
	int i;

	for (i = 0; i < BITMAP_SIZE * 32; i++) {
		if (!bitmap_test(bitmap, i) && !bitmap_test(bitmap_pagetable_mask, i)) {
			bitmap_set(bitmap, i);
            
            pmm_usage_use(1);
			return (void*) (i * 0x1000);
		}
	}

    pmm_debug_print();
    abortf("Allocation failed!");
	return NULL;
}

void* pmm_alloc_pagetable() {
	int i;

	for (i = 0; i < BITMAP_SIZE * 32; i++) {
		if (!bitmap_test(bitmap, i) && bitmap_test(bitmap_pagetable_mask, i)) {
			bitmap_set(bitmap, i);
            
            pmm_usage_use(1);
			return (void*) (i * 0x1000);
		}
	}

    pmm_debug_print();
    abortf("Allocation failed!");
	return NULL;
}

void pmm_mark_used(void* page) {
	uintptr_t index = (uintptr_t) page / 4096;
	bitmap_set(bitmap, index);

    pmm_usage_use(1);
}

void pmm_free(void* page) {
	uintptr_t index = (uintptr_t) page / 4096;
	bitmap_clear(bitmap, index);

    pmm_usage_unuse(1);
}

void* pmm_alloc_range(int count) {
	for (int i = 0; i < BITMAP_SIZE * 32; i++) {
	retry:
		if (!bitmap_test(bitmap, i) && !bitmap_test(bitmap_pagetable_mask, i)) {
			for (int j = 0; j < count; j++) {
				if (bitmap_test(bitmap, i + j) && !bitmap_test(bitmap_pagetable_mask, i + j)) {
					i += j;
					goto retry;
				}
			}

			for (int j = 0; j < count; j++) {
				bitmap_set(bitmap, i + j);
			}

            pmm_usage_use(count);
			return (void*) (i * 0x1000);
		}
	}

	return NULL;
}

void pmm_free_range(void* page, int count) {
	uintptr_t index = (uintptr_t) page / 4096;
	for (int i = 0; i < count; i++) {
		bitmap_clear(bitmap, index + i);
	}


    pmm_usage_unuse(count);
}