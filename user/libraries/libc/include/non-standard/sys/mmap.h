#pragma once

#include <stdbool.h>

void mmap(void *addr);
void mmmap(void* addr, void* addr_rem, int pid);
bool mmap_mapped(void* address);