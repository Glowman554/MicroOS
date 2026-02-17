#include <stdint.h>
#include <stdbool.h>

#include <string.h>

#include <sys/env.h>
#include <sys/file.h>
#include <sys/exit.h>
#include <sys/mmap.h>

#include <elf.h>


#define write_stdout(msg) write(1, msg, sizeof(msg)-1, 0)


#define ALLOC_BASE 0xA0000000

uint32_t alloc_current = ALLOC_BASE;
uint32_t alloc_mapped_up_to = ALLOC_BASE;

void* alloc(uint32_t size) {
	size = (size + 15) & ~15;

	uint32_t result = alloc_current;
	uint32_t new_end = alloc_current + size;

	while (alloc_mapped_up_to < new_end) {
		mmap((void*)alloc_mapped_up_to);
		alloc_mapped_up_to += 0x1000;
	}

	alloc_current = new_end;
	return (void*)result;
}

void* alloc_zeroed(uint32_t size) {
	void* p = alloc(size);
	memset(p, 0, size);
	return p;
}

#define MAX_OBJECTS 32
#define MAX_GLOBAL_SYMBOLS 4096

typedef struct {
	void* image;
	uint32_t image_size;
	struct elf_header* header;
	struct elf_section_header* sec_hdrs;
	uint32_t* sec_addrs;
	void* load_base;
	uint32_t load_size;
	char* filename;
} loaded_object_t;

typedef struct {
	char* name;
	uint32_t address;
	uint8_t bind;
	uint8_t type; 
	int obj_idx;
} global_symbol_t;

loaded_object_t objects[MAX_OBJECTS];
int object_count = 0;

global_symbol_t global_symbols[MAX_GLOBAL_SYMBOLS];
int global_symbol_count = 0;


void* load_file(char* path, uint32_t* out_size) {
	int fd = open(path, FILE_OPEN_MODE_READ);
	if (fd < 0) {
		write_stdout("load: could not open input file\n");
		exit(1);
	}

	int size = filesize(fd);

	void* buf = alloc(size);
	read(fd, buf, size, 0);
	close(fd);

	*out_size = (uint32_t)size;
	return buf;
}

void load_object(char* path) {
	if (object_count >= MAX_OBJECTS) {
		write_stdout("load: too many object files\n");
		exit(1);
	}

	uint32_t file_size;
	void* image = load_file(path, &file_size);

	struct elf_header* header = (struct elf_header*)image;
	if (header->magic != ELF_MAGIC) {
		write_stdout("load: invalid ELF file\n");
		exit(1);
	}
	if (header->sh_entry_count == 0) {
		write_stdout("load: no section headers\n");
		exit(1);
	}

	struct elf_section_header* sec_hdrs = (struct elf_section_header*)((uint8_t*)image + header->sh_offset);

	uint32_t* sec_addrs = (uint32_t*)alloc_zeroed(header->sh_entry_count * sizeof(uint32_t));
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
		write_stdout("load: no loadable sections\n");
		exit(1);
	}

	void* load_base = alloc_zeroed(current_offset);

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

	/* Store object info */
	loaded_object_t* obj = &objects[object_count];
	obj->image = image;
	obj->image_size = file_size;
	obj->header = header;
	obj->sec_hdrs = sec_hdrs;
	obj->sec_addrs = sec_addrs;
	obj->load_base = load_base;
	obj->load_size = current_offset;
	obj->filename = path;

	object_count++;
}

void register_symbol(char* name, uint32_t address, uint8_t bind, uint8_t type, int obj_idx) {
	for (int i = 0; i < global_symbol_count; i++) {
		if (strcmp(global_symbols[i].name, name) == 0) {
			if (global_symbols[i].bind == STB_WEAK && bind == STB_GLOBAL) {
				global_symbols[i].address = address;
				global_symbols[i].bind = bind;
				global_symbols[i].type = type;
				global_symbols[i].obj_idx = obj_idx;
				return;
			}

			if (bind == STB_WEAK && global_symbols[i].bind == STB_GLOBAL) {
				return;
			}

			if (bind == STB_GLOBAL && global_symbols[i].bind == STB_GLOBAL) {
				return;
			}

			return;
		}
	}

	if (global_symbol_count >= MAX_GLOBAL_SYMBOLS) {
		write_stdout("load: too many global symbols\n");
		exit(1);
	}

	global_symbol_t* sym = &global_symbols[global_symbol_count++];
	sym->name = name;
	sym->address = address;
	sym->bind = bind;
	sym->type = type;
	sym->obj_idx = obj_idx;
}

void collect_symbols(void) {
	for (int obj_idx = 0; obj_idx < object_count; obj_idx++) {
		loaded_object_t* obj = &objects[obj_idx];

		for (uint32_t i = 0; i < obj->header->sh_entry_count; i++) {
			if (obj->sec_hdrs[i].type != SHT_SYMTAB) {
				continue;
			}

			struct elf_symbol* symtab = (struct elf_symbol*)((uint8_t*)obj->image + obj->sec_hdrs[i].offset);
			uint32_t sym_count = obj->sec_hdrs[i].size / sizeof(struct elf_symbol);

			char* strtab = (char*)obj->image + obj->sec_hdrs[obj->sec_hdrs[i].link].offset;

			for (uint32_t j = 0; j < sym_count; j++) {
				struct elf_symbol* sym = &symtab[j];
				uint8_t bind = ELF_ST_BIND(sym->info);
				uint8_t type = ELF_ST_TYPE(sym->info);

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
				} else if (sym->shndx < obj->header->sh_entry_count) {
					address = (uint32_t)obj->load_base + obj->sec_addrs[sym->shndx] + sym->value;
				} else {
					continue;
				}

				register_symbol(name, address, bind, type, obj_idx);
			}
		}
	}
}

uint32_t lookup_global_symbol(char* name) {
	for (int i = 0; i < global_symbol_count; i++) {
		if (strcmp(global_symbols[i].name, name) == 0) {
			return global_symbols[i].address;
		}
	}
	return 0;
}

void relocate_all(void) {
	for (int obj_idx = 0; obj_idx < object_count; obj_idx++) {
		loaded_object_t* obj = &objects[obj_idx];

		for (uint32_t i = 0; i < obj->header->sh_entry_count; i++) {
			struct elf_section_header* reloc_sec = &obj->sec_hdrs[i];

			if (reloc_sec->type != SHT_REL && reloc_sec->type != SHT_RELA) {
				continue;
			}

			if (reloc_sec->info >= obj->header->sh_entry_count ||
			    reloc_sec->link >= obj->header->sh_entry_count) {
				continue;
			}

			struct elf_section_header* target_sec = &obj->sec_hdrs[reloc_sec->info];
			if (!(target_sec->flags & SHF_ALLOC)) {
				continue;
			}

			struct elf_section_header* symtab_hdr = &obj->sec_hdrs[reloc_sec->link];
			struct elf_symbol* symtab = (struct elf_symbol*)((uint8_t*)obj->image + symtab_hdr->offset);
			uint32_t sym_count = symtab_hdr->size / sizeof(struct elf_symbol);

			struct elf_section_header* strtab_hdr = &obj->sec_hdrs[symtab_hdr->link];
			char* strtab = (char*)obj->image + strtab_hdr->offset;

			uint8_t* target_base = (uint8_t*)obj->load_base + obj->sec_addrs[reloc_sec->info];

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
					struct elf_relocation* rel = (struct elf_relocation*)((uint8_t*)obj->image + reloc_sec->offset) + j;
					reloc_type = ELF_R_TYPE(rel->info);
					sym_idx    = ELF_R_SYM(rel->info);
					offset     = rel->offset;
					addend     = *(int32_t*)(target_base + offset);
				} else {
					struct elf_relocation_addend* rela =
						&((struct elf_relocation_addend*)((uint8_t*)obj->image + reloc_sec->offset))[j];
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
						sym_address = lookup_global_symbol(sym_name);
						if (sym_address == 0) {
							if (ELF_ST_BIND(sym->info) == STB_WEAK) {
								sym_address = 0;
							} else {
								int symbol_len = strlen(sym_name);
								write_stdout("load: undefined symbol: ");
								write(1, sym_name, symbol_len, 0);
								write_stdout("\n");
								exit(1);
							}
						}
					} else if (sym->shndx == SHN_ABS) {
						sym_address = sym->value;
					} else if (sym->shndx < obj->header->sh_entry_count) {
						if (ELF_ST_TYPE(sym->info) == STT_SECTION) {
							sym_address = (uint32_t)obj->load_base + obj->sec_addrs[sym->shndx];
						} else {
							sym_address = (uint32_t)obj->load_base + obj->sec_addrs[sym->shndx] + sym->value;
						}
					}
				}

				uint32_t* reloc_target = (uint32_t*)(target_base + offset);
				uint32_t base_address = (uint32_t)obj->load_base;

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
						write_stdout("load: unsupported relocation type");
						exit(1);
						break;
				}
			}
		}
	}
}

typedef void (*entry_fn_t)(void);
uint32_t find_start_symbol(void) {
	uint32_t addr = lookup_global_symbol("_start");
	if (addr != 0) {
		return addr;
	}

	write_stdout("load: no _start or main symbol found in any object file\n");

	return 0;
}

void _start(void) {
	char** argv = (char**)env(SYS_GET_ARGV_ID);

	int argc = 0;
	for (; argv[argc]; argc++);

	if (argc < 2) {
		write_stdout("Usage: load tmp:/file1.o [tmp:/file2.o ...]\n");
		write_stdout("Loads, links, and executes ELF relocatable object files.\n");
		exit(1);
	}

	for (int i = 1; i < argc; i++) {
		load_object(argv[i]);
	}

	collect_symbols();

	relocate_all();

	uint32_t entry = find_start_symbol();
	write_stdout("load: jumping to _start\n");

	entry_fn_t entry_fn = (entry_fn_t)entry;
	entry_fn();

	exit(0);
}
