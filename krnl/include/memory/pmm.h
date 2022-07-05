#pragma once

#define BITMAP_SIZE 32768

void pmm_init();

void* pmm_alloc();
void pmm_free(void* page);

void* pmm_alloc_range(int count);
void pmm_free_range(void* page, int count);