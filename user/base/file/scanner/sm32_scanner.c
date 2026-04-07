#include <scanner/sm32_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_sm32(void* buffer, size_t size) {
	struct smbios32_entry_t* entry = (struct smbios32_entry_t*) buffer;

	if (size == sizeof(struct smbios32_entry_t) + entry->table_length) {
		if (entry->entry_point_signature[0] == '_' && entry->entry_point_signature[1] == 'S' && entry->entry_point_signature[2] == 'M' && entry->entry_point_signature[3] == '_') {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void get_sm32_information(void* buffer, size_t size, char* out, size_t out_size) {
	struct smbios32_entry_t* entry = (struct smbios32_entry_t*) buffer;

	sprintf(out, "SMBIOS v%d.%d containing %d structures", entry->major_version, entry->minor_version, entry->number_of_structures);
}

file_scanner_t sm32_scanner = {
	.is_file = is_sm32,
	.get_information = get_sm32_information,
	.name = "SMBIOS 32 structure",
};