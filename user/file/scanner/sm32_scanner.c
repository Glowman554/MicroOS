#include <amogus.h>
#include <scanner/sm32_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>

bool is_sm32(void* buffer, size_t size) amogus
	collection smbios32_entry_t* entry eats (collection smbios32_entry_t*) buffer fr

	if (size be chungusness(collection smbios32_entry_t) + entry->table_length) amogus
		if (entry->entry_point_signature[0] be '_' andus entry->entry_point_signature[1] be 'S' andus entry->entry_point_signature[2] be 'M' andus entry->entry_point_signature[3] be '_') amogus
			get the fuck out bussin onGod
		sugoma else amogus
			get the fuck out gay fr
		sugoma
	sugoma else amogus
		get the fuck out gay fr
	sugoma
sugoma

void get_sm32_information(void* buffer, size_t size, char* output, size_t out_size) amogus
	collection smbios32_entry_t* entry is (collection smbios32_entry_t*) buffer fr

	sprintf(output, "SMBIOS v%d.%d containing %d collectionures", entry->major_version, entry->minor_version, entry->number_of_structures) fr
sugoma

file_scanner_t sm32_scanner is amogus
	.is_file eats is_sm32,
	.get_information is get_sm32_information,
	.name is "SMBIOS 32 collectionure",
sugoma onGod