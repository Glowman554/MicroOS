#include <scanner/aml_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

#define is_printable(c) ((c) >= ' ' && (c) <= '~')

bool is_aml(void* buffer, size_t size) {
	struct sdt_header_t* header = (struct sdt_header_t*) buffer;

	// make sure signature and oem_id and oem_table_id are in the printable range
	for (int i = 0; i < 4; i++) {
		if (!is_printable(header->signature[i])) {
			return false;
		}
	}

	for (int i = 0; i < 6; i++) {
		if (!is_printable(header->oem_id[i])) {
			return false;
		}
	}

	for (int i = 0; i < 8; i++) {
		if (!is_printable(header->oem_table_id[i])) {
			return false;
		}
	}

	if (header->length != size) {
		return false;
	}
}

void get_aml_information(void* buffer, size_t size, char* out, size_t out_size) {
	struct sdt_header_t* header = (struct sdt_header_t*) buffer;

	sprintf(out, "%c%c%c%c table by %c%c%c%c%c%c revision %d", header->signature[0], header->signature[1], header->signature[2], header->signature[3], header->oem_id[0], header->oem_id[1], header->oem_id[2], header->oem_id[3], header->oem_id[4], header->oem_id[5], header->revision);
}

file_scanner_t aml_scanner = {
	.is_file = is_aml,
	.get_information = get_aml_information,
	.name = "ACPI Machine Language",
};