#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define ELF_MAGIC 0x464C457F

struct elf_header {
	uint32_t magic;
	uint32_t version;
	uint64_t reserved;
	uint64_t version2;
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
} __attribute__((packed));

struct elf_program_header {
	uint32_t type;
	uint32_t offset;
	uint32_t virt_addr;
	uint32_t phys_addr;
	uint32_t file_size;
	uint32_t mem_size;
	uint32_t flags;
	uint32_t alignment;
} __attribute__((packed));

struct elf_section_header {
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
} __attribute__((packed));

struct elf_symbol {
	uint32_t name;
	uint32_t value;
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t shndx;
} __attribute__((packed));

struct elf_relocation {
	uint32_t offset;
	uint32_t info;
} __attribute__((packed));

struct elf_relocation_addend {
	uint32_t offset;
	uint32_t info;
	int32_t addend;
} __attribute__((packed));

#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_REL 9
#define SHT_RELA 4
#define SHT_NOBITS 8 

#define SHF_ALLOC 2

#define SHN_UNDEF 0
#define SHN_ABS 0xFFF1

#define ELF_ST_BIND(info) ((info) >> 4)
#define ELF_ST_TYPE(info) ((info) & 0xf)
#define ELF_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4

#define STB_LOCAL   0
#define STB_GLOBAL  1
#define STB_WEAK    2

#define R_386_NONE 0
#define R_386_32 1
#define R_386_PC32 2
#define R_386_GOT32 3
#define R_386_PLT32 4
#define R_386_COPY 5
#define R_386_GLOB_DAT 6
#define R_386_JUMP_SLOT 7
#define R_386_RELATIVE 8
#define R_386_GOTOFF 9
#define R_386_GOTPC 10
#define R_386_32PLT 11
#define R_386_TLS_TPOFF 14
#define R_386_TLS_IE 15
#define R_386_TLS_GOTIE 16
#define R_386_TLS_LE 17
#define R_386_TLS_GD 18
#define R_386_TLS_LDM 19
#define R_386_TLS_LE32 35
#define R_386_IRELATIVE 42

#define ELF_R_SYM(info) ((info) >> 8)
#define ELF_R_TYPE(info) ((uint8_t)(info))
#define ELF_R_INFO(sym, type) (((sym) << 8) + (type))

#endif