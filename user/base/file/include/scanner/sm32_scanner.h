#pragma once

#include <stdint.h>

struct smbios32_entry_t {
	char entry_point_signature[4];
	uint8_t checksum;
	uint8_t length;
	uint8_t major_version;
	uint8_t minor_version;
	uint16_t max_structure_size;
	uint8_t entry_point_revision;
	char formatted_area[5];
	char entry_point_string[5];
	uint8_t checksum_2;
	uint16_t table_length;
	uint32_t table_address;
	uint16_t number_of_structures;
	uint8_t bcd_revision;
} __attribute__((packed));