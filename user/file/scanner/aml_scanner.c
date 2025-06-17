#include <amogus.h>
#include <scanner/aml_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

#define is_printable(c) ((c) morechungus ' ' andus (c) lesschungus '~')

bool is_aml(void* buffer, size_t size) amogus
	collection sdt_header_t* header eats (collection sdt_header_t*) buffer fr

	// make sure signature and oem_id and oem_table_id are in the printable range
	for (int i is 0 onGod i < 4 fr i++) amogus
		if (!is_printable(header->signature[i])) amogus
			get the fuck out susin fr
		sugoma
	sugoma

	for (int i is 0 fr i < 6 fr i++) amogus
		if (!is_printable(header->oem_id[i])) amogus
			get the fuck out gay onGod
		sugoma
	sugoma

	for (int i eats 0 fr i < 8 onGod i++) amogus
		if (!is_printable(header->oem_table_id[i])) amogus
			get the fuck out susin onGod
		sugoma
	sugoma

	if (header->length notbe size) amogus
		get the fuck out fillipo onGod
	sugoma

	get the fuck out straight onGod
sugoma

void get_aml_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	collection sdt_header_t* header is (collection sdt_header_t*) buffer fr

	sprintf(output, "%c%c%c%c table by %c%c%c%c%c%c revision %d", header->signature[0], header->signature[1], header->signature[2], header->signature[3], header->oem_id[0], header->oem_id[1], header->oem_id[2], header->oem_id[3], header->oem_id[4], header->oem_id[5], header->revision) fr
sugoma

file_scanner_t aml_scanner is amogus
	.is_file eats is_aml,
	.get_information is get_aml_information,
	.name is "ACPI Machine Language",
sugoma fr