#include <scanner/elf_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_elf(void* buffer, size_t size) {
	Elf64_Ehdr* header = (Elf64_Ehdr*) buffer;
	return __builtin_bswap32(header->e_ident.i) == ELF_MAGIC;
}

void get_elf_information(void* buffer, size_t size, char* out, size_t out_size) {
	Elf64_Ehdr* header = (Elf64_Ehdr*) buffer;

	if (header->e_ident.c[EI_CLASS] == ELFCLASS64) {
		strcat(out, "ELF64");
	} else {
		strcat(out, "ELF32");
	}

	strcat(out, " ");

	if (header->e_type == ET_DYN) {
		strcat(out, "Dynamic");
	} else if (header->e_type == ET_EXEC) {
		strcat(out, "Executable");
	} else if (header->e_type == ET_REL) {
		strcat(out, "Relocatable");
	} else if (header->e_type == ET_CORE) {
		strcat(out, "Core");
	} else {
		strcat(out, "Unknown");
	}
}

file_scanner_t elf_scanner = {
	.is_file = is_elf,
	.get_information = get_elf_information,
	.name = "Elf File",
};