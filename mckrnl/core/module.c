#include <module.h>
#include <fs/initrd.h>
#include <utils/trace.h>
#include <memory/vmm.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

module_t* loaded_modules[MAX_MODULES] = { 0 };
int loaded_module_count = 0;

void* resolve_symbol_address(elf_object_context_t* ctx, const char* symbol_name) {
    for (uint32_t i = 0; i < ctx->header->sh_entry_count; i++) {
        if (ctx->section_headers[i].type != SHT_SYMTAB) {
            continue;
        }

        struct elf_symbol* symbol_table = (struct elf_symbol*)((uint8_t*)ctx->image + ctx->section_headers[i].offset);
        uint32_t symbol_count = ctx->section_headers[i].size / sizeof(struct elf_symbol);
        char* string_table = (char*)ctx->image + ctx->section_headers[ctx->section_headers[i].link].offset;

        for (uint32_t j = 0; j < symbol_count; j++) {
            if (!strcmp(string_table + symbol_table[j].name, symbol_name)) {
                return (uint8_t*)ctx->load_base + ctx->section_addresses[symbol_table[j].shndx] + symbol_table[j].value;
            }
        }
    }

    return NULL;
}

char* resolve_symbol_name(elf_object_context_t* ctx, void* address) {
    for (uint32_t i = 0; i < ctx->header->sh_entry_count; i++) {
        if (ctx->section_headers[i].type != SHT_SYMTAB) {
            continue;
        }

        struct elf_symbol* symbol_table = (struct elf_symbol*)((uint8_t*)ctx->image + ctx->section_headers[i].offset);
        uint32_t symbol_count = ctx->section_headers[i].size / sizeof(struct elf_symbol);
        char* string_table = (char*)ctx->image + ctx->section_headers[ctx->section_headers[i].link].offset;

        for (uint32_t j = 0; j < symbol_count; j++) {
            void* sym_addr = (uint8_t*)ctx->load_base + ctx->section_addresses[symbol_table[j].shndx] + symbol_table[j].value;
    		if(address >= sym_addr &&  address < sym_addr + symbol_table[j].size) {
                return string_table + symbol_table[j].name;
            }
        }
    }

    return NULL;
}

module_t* load_object_file(void* image) {
    struct elf_header* header = image;
    if (header->magic != ELF_MAGIC || header->sh_entry_count == 0) {
        abortf(false, "Cannot load object file: invalid ELF header");
    }
    debugf("Loading object file at 0x%x", image);
    struct elf_section_header* section_headers = (struct elf_section_header*)((uint8_t*)image + header->sh_offset);
    uint32_t current_offset = 0;
    uint32_t section_addresses[header->sh_entry_count];
    memset(section_addresses, 0, sizeof(section_addresses));

    for (uint32_t i = 0; i < header->sh_entry_count; i++) {
        struct elf_section_header* section = &section_headers[i];

        if (!(section->flags & SHF_ALLOC)) {
            continue;
        }

        if (section->addralign > 1) {
            current_offset = (current_offset + section->addralign - 1) & ~(section->addralign - 1);
        }

        section_addresses[i] = current_offset;
        current_offset += section->size;
    }

    if (current_offset == 0) {
        abortf(false, "Object file has no loadable sections");
    }

    uint32_t pages = (current_offset + 4095) / 4096;
    void* load_base = vmm_alloc(pages);

    memset(load_base, 0, pages * 4096);

    debugf("Allocated %d pages for object file at %x%x", pages, load_base);

    for (uint32_t i = 0; i < header->sh_entry_count; i++) {
        struct elf_section_header* section = &section_headers[i];

        if (!(section->flags & SHF_ALLOC)) {
            continue;
        }

        void* destination = (uint8_t*)load_base + section_addresses[i];

        if (section->type == SHT_PROGBITS) {
            // debugf("Loading section %d to 0x%x", i, destination);
            memcpy(destination, (uint8_t*)image + section->offset, section->size);
        } else if (section->type == SHT_NOBITS) {
            // debugf("Zeroing section %d at 0x%x", i, destination);
            memset(destination, 0, section->size);
        }
    }

    for (uint32_t i = 0; i < header->sh_entry_count; i++) {
        struct elf_section_header* reloc_section = &section_headers[i];

        if (reloc_section->type != SHT_REL && reloc_section->type != SHT_RELA) {
            continue;
        }

        if (reloc_section->info >= header->sh_entry_count || reloc_section->link >= header->sh_entry_count) {
            continue;
        }

        struct elf_section_header* target_section = &section_headers[reloc_section->info];

        if (!(target_section->flags & SHF_ALLOC)) {
            continue;
        }

        struct elf_section_header* symbol_table_header = &section_headers[reloc_section->link];
        struct elf_symbol* symbol_table = (struct elf_symbol*)((uint8_t*)image + symbol_table_header->offset);
        uint32_t symbol_count = symbol_table_header->size / sizeof(struct elf_symbol);

        struct elf_section_header* string_table_header = &section_headers[symbol_table_header->link];
        char* string_table = (char*)image + string_table_header->offset;

        uint8_t* target_base = (uint8_t*)load_base + section_addresses[reloc_section->info];

        uint32_t relocation_entry_size;
        if (reloc_section->type == SHT_REL) {
            relocation_entry_size = sizeof(struct elf_relocation);
        } else {
            relocation_entry_size = sizeof(struct elf_relocation_addend);
        }
        uint32_t relocation_count = reloc_section->size / relocation_entry_size;

        for (uint32_t j = 0; j < relocation_count; j++) {
            uint32_t reloc_type, symbol_index, addend, offset;

            if (reloc_section->type == SHT_REL) {
                struct elf_relocation* relocation = &((struct elf_relocation*)((uint8_t*)image + reloc_section->offset))[j];
                reloc_type = ELF_R_TYPE(relocation->info);
                symbol_index = ELF_R_SYM(relocation->info);
                offset = relocation->offset;
                addend = *(uint32_t*)(target_base + offset);
            } else {
                struct elf_relocation_addend* relocation = &((struct elf_relocation_addend*)((uint8_t*)image + reloc_section->offset))[j];
                reloc_type = ELF_R_TYPE(relocation->info);
                symbol_index = ELF_R_SYM(relocation->info);
                offset = relocation->offset;
                addend = relocation->addend;
            }

            if (symbol_index >= symbol_count) {
                continue;
            }

            struct elf_symbol* symbol = &symbol_table[symbol_index];
            char* symbol_name = string_table + symbol->name;

            uint32_t symbol_address = 0;
            if (symbol_index != 0) {
                if (!strcmp(symbol_name, "_GLOBAL_OFFSET_TABLE_")) {
                    abortf(false, "_GLOBAL_OFFSET_TABLE_ symbol is not supported");
                } else if (symbol->shndx == SHN_UNDEF) {
                    symbol_address = resolve_symbol_from_name(symbol_name);
                    if (symbol_address == 0) {
                        abortf(false, "Could not resolve symbol %s", symbol_name);
                    }
                    // debugf("Resolved %s to address 0x%x", symbol_name, symbol_address);
                } else if (symbol->shndx == SHN_ABS) {
                    symbol_address = symbol->value;
                } else if (symbol->shndx < header->sh_entry_count) {
                    if (ELF_ST_TYPE(symbol->info) == STT_SECTION) {
                        symbol_address = (uint32_t)load_base + section_addresses[symbol->shndx];
                    } else {
                        symbol_address = (uint32_t)load_base + section_addresses[symbol->shndx] + symbol->value;
                    }
                }
            }

            uint32_t* reloc_target = (uint32_t*)(target_base + offset);
            uint32_t base_address = (uint32_t)load_base;

            switch (reloc_type) {
                case R_386_NONE:
                    break;
                case R_386_32:
                    *reloc_target = symbol_address + addend;
                    break;
                case R_386_PC32:
                case R_386_PLT32:
                    *reloc_target = symbol_address + addend - (uint32_t)reloc_target;
                    break;
                case R_386_GLOB_DAT:
                case R_386_JUMP_SLOT:
                    *reloc_target = symbol_address;
                    break;
                case R_386_RELATIVE:
                case R_386_IRELATIVE:
                    *reloc_target = base_address + addend;
                    break;
                case R_386_TLS_LE:
                case R_386_TLS_LE32:
                case R_386_TLS_IE:
                case R_386_TLS_GOTIE:
                    *reloc_target = symbol_address + addend;
                    break;
                default:
                    abortf(false, "Unsupported relocation type %d in object file", reloc_type);
                    break;
            }
        }
    }

    elf_object_context_t ctx = {
        .image = image,
        .header = header,
        .section_headers = section_headers,
        .section_addresses = section_addresses,
        .load_base = load_base
    };

    module_t* module = (module_t*) resolve_symbol_address(&ctx, "__module__");
    if (module) {
        debugf("Found module structure at 0x%x", module);
        module->ctx = ctx;
    } else {
        abortf(false, "No __module__ symbol found in object file");
    }

    return module;
}


void initrd_load_modules(void* saf_image, char* path) {
	saf_node_hdr_t* node = initrd_find(path, saf_image, (saf_node_hdr_t*) saf_image);
	if (node == NULL) {
		abortf(false, "initrd: could not find modules folder at %s", path);
		return;
	}

	if (node->flags != FLAG_ISFOLDER) {
		abortf(false, "initrd: modules path %s is not a folder", path);
		return;
	}

	saf_node_folder_t* folder_node = (saf_node_folder_t*) node;

	for (int i = 0; i < folder_node->num_children; i++) {
		saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) saf_image + (uint32_t) folder_node->children[i]);
		if (child->flags == FLAG_ISFOLDER) {
			continue;
		}

		saf_node_file_t* file_node = (saf_node_file_t*) child;
		void* module_data = (void*) ((uint32_t) saf_image + (uint32_t) file_node->addr);

		debugf("initrd: loading module %s (%d bytes)", child->name, file_node->size);
		module_t* module = load_object_file(module_data);

        loaded_modules[loaded_module_count++] = module;

        debugf("Loaded module %s", module->name);

		if (module->init) {
			module->init();
		}

	}
}

void stage_driver() {
	for (int i = 0; i < loaded_module_count; i++) {
		module_t* module = loaded_modules[i];
		if (module->stage_driver) {
			module->stage_driver();
		}
	}
}

void stage_mount() {
	for (int i = 0; i < loaded_module_count; i++) {
		module_t* module = loaded_modules[i];
		if (module->stage_mount) {
			module->stage_mount();
		}
	}
}

