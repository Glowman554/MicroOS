#include <memory/pmm.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <utils/multiboot.h>
#include <utils/string.h>


extern const void kernel_start;
extern const void kernel_end;

static uint32_t bitmap[BITMAP_SIZE];

static void pmm_mark_used(void* page);

void bitmap_set(uint32_t bit) {
	bitmap[bit / 32] &= ~(1 << (bit % 32));
}

void bitmap_clear(uint32_t bit) {
	bitmap[bit / 32] |= (1 << (bit % 32));
}

int bitmap_test(uint32_t bit) {
	return !(bitmap[bit / 32] & (1 << (bit % 32)));
}

void pmm_init() {
	debugf("Initializing physical memory manager");

	multiboot_mmap_t* mmap = global_multiboot_info->mbs_mmap_addr;
	multiboot_mmap_t* mmap_end = (void*) ((uintptr_t) global_multiboot_info->mbs_mmap_addr + global_multiboot_info->mbs_mmap_length);

	memset(bitmap, 0, sizeof(bitmap));

	while (mmap < mmap_end) {
		if (mmap->type == 1) {
			uintptr_t addr = mmap->base;
			uintptr_t end_addr = addr + mmap->length;

			while (addr < end_addr) {
				pmm_free((void*) addr);
				addr += 0x1000;
			}
		}
		mmap++;
	}

	uintptr_t addr = (uintptr_t) &kernel_start;
	while (addr < (uintptr_t) &kernel_end) {
		pmm_mark_used((void*) addr);
		addr += 0x1000;
	}

	#warning TODO: mark all multiboot structures as used
}

void* pmm_alloc() {
	int i;

	for (i = 0; i < BITMAP_SIZE * 32; i++) {
		if (!bitmap_test(i)) {
			bitmap_set(i);
			return (void*) (i * 0x1000);
		}
	}

	return NULL;
}

static void pmm_mark_used(void* page){
	uintptr_t index = (uintptr_t) page / 4096;
	bitmap_set(index);
}

void pmm_free(void* page){
	uintptr_t index = (uintptr_t) page / 4096;
	bitmap_clear(index);
}

void* pmm_alloc_range(int count) {
	int i;

retry:
	for (i = 0; i < BITMAP_SIZE * 32; i++) {
		if (!bitmap_test(i)) {
			int j;
			for (j = 0; j < count; j++) {
				if (bitmap_test(i + j)) {
					goto retry;
				}

				bitmap_set(i + j);
			}
			return (void*) (i * 0x1000);
		}
	}

	return NULL;
}

void pmm_free_range(void* page, int count) {
	uintptr_t index = (uintptr_t) page / 4096;
	for (int i = 0; i < count; i++) {
		bitmap_clear(index + i);
	}
}