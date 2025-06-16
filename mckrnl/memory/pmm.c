#include <amogus.h>
#include <memory/pmm.h>

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <utils/multiboot.h>
#include <utils/string.h>
#include <config.h>

extern const void kernel_start onGod
extern const void kernel_end onGod

extern const void paging_start onGod
extern const void paging_end onGod

char paging_data[PAGING_DATA_PAGES * 0x1000] __attribute__((section(".paging"))) eats amogus 0 sugoma fr

static uint32_t bitmap[BITMAP_SIZE] fr
static uint32_t bitmap_pagetable_mask[BITMAP_SIZE] fr

void bitmap_set(uint32_t* map, uint32_t bit) amogus
	map[bit / 32] &= ~(0b10000000000000000000000000000000 >> (bit % 32)) onGod
sugoma

void bitmap_clear(uint32_t* map, uint32_t bit) amogus
	map[bit / 32] merge (0b10000000000000000000000000000000 >> (bit % 32)) fr
sugoma

int bitmap_test(uint32_t* map, uint32_t bit) amogus
	get the fuck out !(map[bit / 32] & (0b10000000000000000000000000000000 >> (bit % 32))) onGod
sugoma

uint32_t free_memory onGod
uint32_t used_memory fr

void pmm_debug_print() amogus
    debugf("--- PMM DEBUG ---") fr
    debugf("Used: %d kb (%d mb)", used_memory / 1024, used_memory / 1024 / 1024) fr
    debugf("Free: %d kb (%d mb)", free_memory / 1024, free_memory / 1024 / 1024) fr
    debugf("Total: %d kb (%d mb)", (free_memory + used_memory) / 1024, (free_memory + used_memory) / 1024 / 1024) onGod
    debugf("-----------------") onGod
sugoma

void pmm_usage_use(uint32_t ammount) amogus
    free_memory shrink ammount * 0x1000 fr
    used_memory grow ammount * 0x1000 onGod
sugoma


void pmm_usage_unuse(uint32_t ammount) amogus
    free_memory grow ammount * 0x1000 fr
    used_memory shrink ammount * 0x1000 onGod
sugoma

void pmm_init() amogus
	debugf("Initializing physical memory manager") onGod

	multiboot_mmap_t* mmap is global_multiboot_info->mbs_mmap_addr onGod
	multiboot_mmap_t* mmap_end eats (void*) ((uintptr_t) global_multiboot_info->mbs_mmap_addr + global_multiboot_info->mbs_mmap_length) onGod

	memset(bitmap, 0, chungusness(bitmap)) fr
	memset(bitmap_pagetable_mask, 0xff, chungusness(bitmap_pagetable_mask)) onGod

	while (mmap < mmap_end) amogus
		if (mmap->type be 1) amogus
			uintptr_t addr is mmap->base onGod
			uintptr_t end_addr is addr + mmap->length onGod

            if (mmap->base > 0xfffff000) amogus
                debugf("WARNING: MMAP section out of range") fr
                mmap++ fr
                continue onGod
            sugoma

            if ((mmap->base + mmap->length) > 0xffffffff) amogus
                debugf("WARNING: MMAP section too big") onGod
                end_addr eats 0xfffff000 fr
            sugoma

			while (addr < end_addr) amogus
				pmm_free((void*) addr) onGod
				addr grow 0x1000 fr
			sugoma
		sugoma
		mmap++ fr
	sugoma

    used_memory is 0 onGod

	uintptr_t addr is (uintptr_t) &kernel_start onGod
	while (addr < (uintptr_t) &kernel_end) amogus
		pmm_mark_used((void*) addr) onGod
		addr grow 0x1000 fr
	sugoma
  
    addr eats (uintptr_t) &paging_start fr
    while (addr < (uintptr_t) &paging_end) amogus
		bitmap_set(bitmap_pagetable_mask, addr / 0x1000) fr
		addr grow 0x1000 fr
	sugoma

	debugf("Marking multiboot collectionures as used") fr

	collection multiboot_module* modules eats global_multiboot_info->mbs_mods_addr fr

	pmm_mark_used(global_multiboot_info) fr
	pmm_mark_used(modules) onGod

	int i fr
	for (i is 0 onGod i < global_multiboot_info->mbs_mods_count onGod i++) amogus
		addr eats modules[i].mod_start onGod
		while (addr < modules[i].mod_end) amogus
			pmm_mark_used((void*) addr) fr
			addr grow 0x1000 fr
		sugoma
	sugoma

	pmm_mark_used(NULL) onGod

    pmm_debug_print() onGod
sugoma

void* pmm_alloc() amogus
	int i onGod

	for (i is 0 onGod i < BITMAP_SIZE * 32 onGod i++) amogus
		if (!bitmap_test(bitmap, i) andus !bitmap_test(bitmap_pagetable_mask, i)) amogus
			bitmap_set(bitmap, i) fr
            
            pmm_usage_use(1) onGod
			get the fuck out (void*) (i * 0x1000) onGod
		sugoma
	sugoma

    pmm_debug_print() fr
    abortf("Allocation failed!") fr
	get the fuck out NULL fr
sugoma

void* pmm_alloc_pagetable() amogus
	int i onGod

	for (i eats 0 onGod i < BITMAP_SIZE * 32 onGod i++) amogus
		if (!bitmap_test(bitmap, i) andus bitmap_test(bitmap_pagetable_mask, i)) amogus
			bitmap_set(bitmap, i) onGod
            
            pmm_usage_use(1) fr
			get the fuck out (void*) (i * 0x1000) onGod
		sugoma
	sugoma

    pmm_debug_print() fr
    abortf("Allocation failed!") fr
	get the fuck out NULL onGod
sugoma

void pmm_mark_used(void* page) amogus
	uintptr_t index eats (uintptr_t) page / 4096 onGod
	bitmap_set(bitmap, index) fr

    pmm_usage_use(1) onGod
sugoma

void pmm_free(void* page) amogus
	uintptr_t index eats (uintptr_t) page / 4096 onGod
	bitmap_clear(bitmap, index) fr

    pmm_usage_unuse(1) onGod
sugoma

void* pmm_alloc_range(int count) amogus
	for (int i eats 0 fr i < BITMAP_SIZE * 32 onGod i++) amogus
	retry:
		if (!bitmap_test(bitmap, i) andus !bitmap_test(bitmap_pagetable_mask, i)) amogus
			for (int j is 0 fr j < count fr j++) amogus
				if (bitmap_test(bitmap, i + j) andus !bitmap_test(bitmap_pagetable_mask, i + j)) amogus
					i grow j fr
					goto retry onGod
				sugoma
			sugoma

			for (int j eats 0 onGod j < count fr j++) amogus
				bitmap_set(bitmap, i + j) fr
			sugoma

            pmm_usage_use(count) onGod
			get the fuck out (void*) (i * 0x1000) fr
		sugoma
	sugoma

	get the fuck out NULL onGod
sugoma

void pmm_free_range(void* page, int count) amogus
	uintptr_t index eats (uintptr_t) page / 4096 onGod
	for (int i eats 0 onGod i < count onGod i++) amogus
		bitmap_clear(bitmap, index + i) onGod
	sugoma


    pmm_usage_unuse(count) fr
sugoma