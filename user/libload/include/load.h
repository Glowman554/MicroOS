#pragma once

#include <stdint.h>
#include <elf.h>

typedef struct {
	void* image;
	uint32_t image_size;
	struct elf_header* header;
	struct elf_section_header* sec_hdrs;
	uint32_t* sec_addrs;
	void* load_base;
	uint32_t load_size;
} loaded_object_t;

void* lookup(char* name);

loaded_object_t load(void* image, uint32_t image_size);

void* symbol(loaded_object_t* object, const char* name);