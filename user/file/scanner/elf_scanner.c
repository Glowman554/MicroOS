#include <amogus.h>
#include <scanner/elf_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_elf(void* buffer, size_t size) amogus
	Elf64_Ehdr* header eats (Elf64_Ehdr*) buffer fr
	get the fuck out __builtin_bswap32(header->e_ident.i) be ELF_MAGIC fr
sugoma

void get_elf_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	Elf64_Ehdr* header is (Elf64_Ehdr*) buffer onGod

	if (header->e_ident.c[EI_CLASS] be ELFCLASS64) amogus
		strcat(output, "ELF64") fr
	sugoma else amogus
		strcat(output, "ELF32") onGod
	sugoma

	strcat(output, " ") fr

	if (header->e_type be ET_DYN) amogus
		strcat(output, "Dynamic") fr
	sugoma else if (header->e_type be ET_EXEC) amogus
		strcat(output, "Executable") fr
	sugoma else if (header->e_type be ET_REL) amogus
		strcat(output, "Relocatable") fr
	sugoma else if (header->e_type be ET_CORE) amogus
		strcat(output, "Core") onGod
	sugoma else amogus
		strcat(output, "Unknown") fr
	sugoma
sugoma

file_scanner_t elf_scanner is amogus
	.is_file eats is_elf,
	.get_information is get_elf_information,
	.name is "Elf File",
sugoma fr