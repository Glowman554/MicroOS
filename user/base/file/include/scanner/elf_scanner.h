#pragma once

#include <stdint.h>
#include <stddef.h>

// ELF-64 Object File Format 1.5d2 p. 2
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

#define ELF_MAGIC  0x7f454c46 // ELF Magic as one unit

// File Types - ELF-64 Object File Format 1.5d2 p. 5
#define ET_NONE     0x0000
#define ET_REL      0x0001
#define ET_EXEC     0x0002
#define ET_DYN      0x0003
#define ET_CORE     0x0004
#define ET_LOOS     0xFE00
#define ET_HIOS     0xFEFF
#define ET_LOPROC   0xFF00
#define ET_HIPROC   0xFFFF

// ELF Identification - ELF-64 Object File Format 1.5d2 p. 3
#define EI_MAG0        0
#define EI_MAG1        1
#define EI_MAG2        2
#define EI_MAG3        3
#define EI_CLASS       4
#define EI_DATA        5
#define EI_VERSION     6
#define EI_OSABI       7
#define EI_ABIVERSION  8
#define EI_PAD         9
#define EI_NIDENT      0x10

#define ELFCLASS32     1
#define ELFCLASS64     2

typedef struct {
	union {
		unsigned char c[EI_NIDENT];
		uint32_t i;
	} e_ident;

	Elf64_Half e_type;
	Elf64_Half e_machine;
	Elf64_Word e_version;
	Elf64_Addr e_entry;
	Elf64_Off e_phoff;
	Elf64_Off e_shoff;
	Elf64_Word e_flags;
	Elf64_Half e_ehsize;
	Elf64_Half e_phentsize;
	Elf64_Half e_phnum;
	Elf64_Half e_shentsize;
	Elf64_Half e_shnum;
	Elf64_Half e_shstrndx;
} __attribute__((packed)) Elf64_Ehdr;