#pragma once

#include <stdint.h>
#include <stddef.h>

#define ELF_MAGIC       0x464C457F

#define ET_NONE  0
#define ET_REL   1
#define ET_EXEC  2
#define ET_DYN   3

#define EM_386   3

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_NOBITS   8
#define SHT_REL      9

#define SHF_WRITE 1
#define SHF_ALLOC 2
#define SHF_EXEC  4

#define SHN_UNDEF 0
#define SHN_ABS   0xFFF1

#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4

#define ELF_ST_BIND(info)       ((info) >> 4)
#define ELF_ST_TYPE(info)       ((info) & 0xf)
#define ELF_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))

#define ELF_R_SYM(info)        ((info) >> 8)
#define ELF_R_TYPE(info)        ((uint8_t)(info))
#define ELF_R_INFO(sym, type)  (((sym) << 8) + (type))

#define R_386_NONE      0
#define R_386_32        1
#define R_386_PC32      2
#define R_386_PLT32     4

#define PT_NULL  0
#define PT_LOAD  1

#define PF_X 1
#define PF_W 2
#define PF_R 4


typedef struct {
	uint32_t magic;
	uint8_t  e_class;
	uint8_t  data;
	uint8_t  hdr_version;
	uint8_t  os_abi;
	uint8_t  padding[8];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint32_t entry;
	uint32_t ph_offset;
	uint32_t sh_offset;
	uint32_t flags;
	uint16_t header_size;
	uint16_t ph_entry_size;
	uint16_t ph_entry_count;
	uint16_t sh_entry_size;
	uint16_t sh_entry_count;
	uint16_t sh_str_table_index;
} __attribute__((packed)) elf32_ehdr_t;

typedef struct {
	uint32_t type;
	uint32_t offset;
	uint32_t virt_addr;
	uint32_t phys_addr;
	uint32_t file_size;
	uint32_t mem_size;
	uint32_t flags;
	uint32_t alignment;
} __attribute__((packed)) elf32_phdr_t;

typedef struct {
	uint32_t name;
	uint32_t type;
	uint32_t flags;
	uint32_t addr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t addralign;
	uint32_t entsize;
} __attribute__((packed)) elf32_shdr_t;

typedef struct {
	uint32_t name;
	uint32_t value;
	uint32_t size;
	uint8_t  info;
	uint8_t  other;
	uint16_t shndx;
} __attribute__((packed)) elf32_sym_t;

typedef struct {
	uint32_t offset;
	uint32_t info;
} __attribute__((packed)) elf32_rel_t;

typedef struct {
	uint32_t offset;
	uint32_t info;
	int32_t  addend;
} __attribute__((packed)) elf32_rela_t;


#define AR_MAGIC "!<arch>\n"
#define AR_MAGIC_LEN 8

typedef struct {
	char name[16];
	char mtime[12];
	char uid[6];
	char gid[6];
	char mode[8];
	char size[10];
	char fmag[2];
} __attribute__((packed)) ar_header_t;


#define DEFAULT_LOAD_ADDR 0xB0000000
#define MAX_INPUT_FILES   16
#define MAX_LIB_PATHS     8
#define MAX_LIBS          8
#define MAX_SECTIONS      1024
#define MAX_SYMBOLS       8192
#define MAX_RELOCS        16384

typedef struct {
	int obj_idx;
	int sec_idx;
	char name[64];
	uint32_t type;
	uint32_t flags;
	uint32_t size;
	uint32_t addralign;
	uint8_t* data;
	uint32_t out_offset;
	uint32_t out_vaddr;
	int out_group;
} input_section_t;

typedef struct {
	char name[128];
	uint32_t value;
	int defined;
	int obj_idx;
	int sec_idx;
	uint8_t bind;
	uint8_t type;
} global_symbol_t;

typedef struct {
	int isec_idx;
	uint32_t offset;
	uint32_t type;
	int sym_idx;
	int32_t addend;
	int has_addend;
} pending_reloc_t;

typedef struct {
	char filename[256];
	uint8_t* data;
	uint32_t size;
	int section_start;
	int section_count;
	int* sym_map;
	int sym_count;
} input_object_t;

typedef struct {
	input_object_t objects[MAX_INPUT_FILES];
	int num_objects;

	input_section_t input_sections[MAX_SECTIONS];
	int num_input_sections;

	global_symbol_t symbols[MAX_SYMBOLS];
	int num_symbols;

	pending_reloc_t relocs[MAX_RELOCS];
	int num_relocs;

	char* lib_paths[MAX_LIB_PATHS];
	int num_lib_paths;

	char* libs[MAX_LIBS];
	int num_libs;

	uint32_t load_addr;
	char* entry_name;
	char* output_file;
} linker_state_t;


void linker_init(linker_state_t* state);
int linker_add_object(linker_state_t* state, const char* filename);
int linker_resolve_libraries(linker_state_t* state);
int linker_layout(linker_state_t* state);
int linker_relocate(linker_state_t* state);
int linker_write_elf(linker_state_t* state, const char* output);

int  linker_find_symbol(linker_state_t* state, const char* name);
int  linker_add_symbol(linker_state_t* state, const char* name, uint32_t value, int defined, int obj_idx, int sec_idx, uint8_t bind, uint8_t type);

