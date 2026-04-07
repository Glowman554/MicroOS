#include <load.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reallocate(loaded_object_t* object) {
	for (uint32_t i = 0; i < object->header->sh_entry_count; i++) {
		struct elf_section_header* reloc_sec = &object->sec_hdrs[i];

		if (reloc_sec->type != SHT_REL && reloc_sec->type != SHT_RELA) {
			continue;
		}

		if (reloc_sec->info >= object->header->sh_entry_count ||
		    reloc_sec->link >= object->header->sh_entry_count) {
			continue;
		}

		struct elf_section_header* target_sec = &object->sec_hdrs[reloc_sec->info];
		if (!(target_sec->flags & SHF_ALLOC)) {
			continue;
		}

        struct elf_section_header* symtab_hdr = &object->sec_hdrs[reloc_sec->link];
		struct elf_symbol* symtab = (struct elf_symbol*)((uint8_t*)object->image + symtab_hdr->offset);
		uint32_t sym_count = symtab_hdr->size / sizeof(struct elf_symbol);

		struct elf_section_header* strtab_hdr = &object->sec_hdrs[symtab_hdr->link];
		char* strtab = (char*)object->image + strtab_hdr->offset;

		uint8_t* target_base = (uint8_t*)object->load_base + object->sec_addrs[reloc_sec->info];

		uint32_t entry_size;
		if (reloc_sec->type == SHT_REL) {
			entry_size = sizeof(struct elf_relocation);
		} else {
			entry_size = sizeof(struct elf_relocation_addend);
		}
		uint32_t reloc_count = reloc_sec->size / entry_size;

        for (uint32_t j = 0; j < reloc_count; j++) {
			uint32_t reloc_type, sym_idx, offset;
			int32_t addend;

			if (reloc_sec->type == SHT_REL) {
				struct elf_relocation* rel = (struct elf_relocation*)((uint8_t*)object->image + reloc_sec->offset) + j;
				reloc_type = ELF_R_TYPE(rel->info);
				sym_idx    = ELF_R_SYM(rel->info);
				offset     = rel->offset;
				addend     = *(int32_t*)(target_base + offset);
			} else {
				struct elf_relocation_addend* rela =
					&((struct elf_relocation_addend*)((uint8_t*)object->image + reloc_sec->offset))[j];
				reloc_type = ELF_R_TYPE(rela->info);
				sym_idx    = ELF_R_SYM(rela->info);
				offset     = rela->offset;
				addend     = rela->addend;
			}

            if (sym_idx >= sym_count) {
				continue;
			}

			struct elf_symbol* sym = &symtab[sym_idx];
			char* sym_name = strtab + sym->name;

			uint32_t sym_address = 0;
			if (sym_idx != 0) {
				if (sym->shndx == SHN_UNDEF) {
					sym_address = (uint32_t) lookup(sym_name);
					if (sym_address == 0) {
						if (ELF_ST_BIND(sym->info) == STB_WEAK) {
							sym_address = 0;
						} else {
                            printf("load: undefined symbol '%s'\n", sym_name);
                            abort();
						}
					}
				} else if (sym->shndx == SHN_ABS) {
					sym_address = sym->value;
				} else if (sym->shndx < object->header->sh_entry_count) {
					if (ELF_ST_TYPE(sym->info) == STT_SECTION) {
						sym_address = (uint32_t)object->load_base + object->sec_addrs[sym->shndx];
					} else {
						sym_address = (uint32_t)object->load_base + object->sec_addrs[sym->shndx] + sym->value;
					}
				}
			}

			uint32_t* reloc_target = (uint32_t*)(target_base + offset);
			uint32_t base_address = (uint32_t)object->load_base;

			switch (reloc_type) {
				case R_386_NONE:
					break;
				case R_386_32:
					*reloc_target = sym_address + addend;
					break;
				case R_386_PC32:
				case R_386_PLT32:
					*reloc_target = sym_address + addend - (uint32_t)reloc_target;
					break;
				case R_386_GLOB_DAT:
				case R_386_JUMP_SLOT:
					*reloc_target = sym_address;
					break;
				case R_386_RELATIVE:
				case R_386_IRELATIVE:
					*reloc_target = base_address + addend;
					break;
				default:
					printf("load: unsupported relocation type %d\n", reloc_type);
                    abort();
					break;
			}
		}
	}
}

loaded_object_t load(void* image, uint32_t image_size) {
    struct elf_header* header = (struct elf_header*)image;
	if (header->magic != ELF_MAGIC) {
		printf("load: invalid ELF file\n");
        abort();
	}
	if (header->sh_entry_count == 0) {
		printf("load: no section headers\n");
        abort();
	}

	struct elf_section_header* sec_hdrs = (struct elf_section_header*)((uint8_t*)image + header->sh_offset);

	uint32_t* sec_addrs = (uint32_t*) malloc(header->sh_entry_count * sizeof(uint32_t));
    memset(sec_addrs, 0, header->sh_entry_count * sizeof(uint32_t));
	uint32_t current_offset = 0;

	for (uint32_t i = 0; i < header->sh_entry_count; i++) {
		struct elf_section_header* sec = &sec_hdrs[i];
		if (!(sec->flags & SHF_ALLOC)) {
			continue;
		}
		if (sec->addralign > 1) {
			current_offset = (current_offset + sec->addralign - 1) & ~(sec->addralign - 1);
		}
		sec_addrs[i] = current_offset;
		current_offset += sec->size;
	}

	if (current_offset == 0) {
		printf("load: no loadable sections\n");
		abort();
	}

	void* load_base = malloc(current_offset);
    memset(load_base, 0, current_offset);

	for (uint32_t i = 0; i < header->sh_entry_count; i++) {
		struct elf_section_header* sec = &sec_hdrs[i];
		if (!(sec->flags & SHF_ALLOC)) {
			continue;
		}

		void* dest = (uint8_t*)load_base + sec_addrs[i];
		if (sec->type == SHT_PROGBITS) {
			memcpy(dest, (uint8_t*)image + sec->offset, sec->size);
		}
	}

	loaded_object_t obj;
	obj.image = image;
	obj.image_size = image_size;
	obj.header = header;
	obj.sec_hdrs = sec_hdrs;
	obj.sec_addrs = sec_addrs;
	obj.load_base = load_base;
	obj.load_size = current_offset;

    reallocate(&obj);

	return obj;
}

void* symbol(loaded_object_t* object, const char* symbol_name) {
	for (uint32_t i = 0; i < object->header->sh_entry_count; i++) {
		if (object->sec_hdrs[i].type != SHT_SYMTAB) {
			continue;
		}

		struct elf_symbol* symtab = (struct elf_symbol*)((uint8_t*)object->image + object->sec_hdrs[i].offset);
		uint32_t sym_count = object->sec_hdrs[i].size / sizeof(struct elf_symbol);

		char* strtab = (char*)object->image + object->sec_hdrs[object->sec_hdrs[i].link].offset;

		for (uint32_t j = 0; j < sym_count; j++) {
			struct elf_symbol* sym = &symtab[j];
	    	uint8_t bind = ELF_ST_BIND(sym->info);

			if (bind != STB_GLOBAL && bind != STB_WEAK) {
				continue;
			}
			if (sym->shndx == SHN_UNDEF) {
				continue;
			}

			char* name = strtab + sym->name;
    		if (name[0] == '\0') {
				continue;
			}

			uint32_t address;
			if (sym->shndx == SHN_ABS) {
				address = sym->value;
			} else if (sym->shndx < object->header->sh_entry_count) {
				address = (uint32_t)object->load_base + object->sec_addrs[sym->shndx] + sym->value;
			} else {
				continue;
			}

            if (strcmp(name, symbol_name) == 0) {
                return (void*)address;
            }
		}
	}

    return NULL;
}