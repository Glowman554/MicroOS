#pragma once

#include <stdint.h>
#include <driver/acpi/tables.h>

typedef struct rsdp2 {
	uint8_t signature[8];
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserved[3];
} __attribute__((packed)) rsdp2_t;

#define RSDP_SCAN_BASE_ADDR 0xE0000
#define RSDP_SCAN_LENGTH 0xFFFFF

extern rsdt_t* rsdt;
extern xsdt_t* xsdt;

rsdp2_t* scan_for_rsdp(char* start, uint32_t length);
void rsdp_init();
void* find_SDT(const char *signature);

void map_sdt(sdt_header_t* header);