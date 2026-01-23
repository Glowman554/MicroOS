#pragma once

#include <scheduler/elf.h>

typedef struct {
    void* image;
    struct elf_header* header;
    struct elf_section_header* section_headers;
    uint32_t* section_addresses;
    void* load_base;
} elf_object_context_t;

typedef struct module {
	const char* name;
	void (*init)();

    void (*stage_driver)();
    void (*stage_mount)();

    // filled by loader
    elf_object_context_t ctx;
} module_t;

#define define_module(name, init, stage_driver, stage_mount) module_t __module__ = { name, init, stage_driver, stage_mount };

module_t* load_object_file(void* image);
void* resolve_symbol_address(elf_object_context_t* ctx, const char* symbol_name);
void initrd_load_modules(void* saf_image, char* path);

void stage_driver();
void stage_mount();
