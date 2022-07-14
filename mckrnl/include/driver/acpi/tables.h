#pragma once

#include <stdint.h>

typedef struct sdt_header {
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed)) sdt_header_t;

typedef struct rsdt {
	sdt_header_t header;
    uint32_t acpiptr[];
} __attribute((packed)) rsdt_t;

typedef struct xsdt {
	sdt_header_t header;
    uint64_t acpiptr[];
} __attribute__((packed)) xsdt_t;

typedef struct hpet_table {
	sdt_header_t header;
	uint8_t hardware_rev_id;
	uint8_t info;
	uint16_t pci_id;
	uint8_t address_space_id;
	uint8_t register_width;
	uint8_t register_offset;
	uint8_t reserved;
	uint64_t address;
	uint8_t hpet_num;
	uint16_t minim_ticks;
	uint8_t page_protection;
} __attribute__((packed)) hpet_table_t;

typedef struct generic_address_structure {
	uint8_t address_space;
	uint8_t bit_width;
	uint8_t bit_offset;
	uint8_t access_size;
	uint64_t address;
} __attribute__((packed)) generic_address_structure_t;

enum generic_address_structure_address_space {
	GENERIC_ADDRESS_SPACE_SYSTEM_MEMORY = 0,
	GENERIC_ADDRESS_SPACE_SYSTEM_IO = 1,
	GENERIC_ADDRESS_SPACE_PCI_CONFIGURATION_SPACE = 2,
	GENERIC_ADDRESS_SPACE_EMBEDDED_CONTROLLER = 3,
	GENERIC_ADDRESS_SPACE_SMBUS = 4
};

typedef struct fadt_table {
	sdt_header_t header;
	uint32_t firmware_ctrl;
	uint32_t dsdt;
	uint8_t reserved;
	uint8_t preferred_power_management_profile;
	uint16_t sci_interrupt;
	uint32_t smi_command_port;
	uint8_t acpi_enable;
	uint8_t acpi_disable;
	uint8_t S4BIOS_REQ;
	uint8_t PSTATE_control;
	uint32_t PM1a_event_block;
	uint32_t PM1b_event_block;
	uint32_t PM1a_control_block;
	uint32_t PM1b_control_block;
	uint32_t PM2_control_block;
	uint32_t PM_timer_block;
	uint32_t GPE0_block;
	uint32_t GPE1_block;
	uint8_t PM1_event_length;
	uint8_t PM1_control_length;
	uint8_t PM2_control_length;
	uint8_t PM_timer_length;
	uint8_t GPE0_length;
	uint8_t GPE1_length;
	uint8_t GPE1_base;
	uint8_t c_state_control;
	uint16_t worst_c2_latency;
	uint16_t worst_c3_latency;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alarm;
	uint8_t month_alarm;
	uint8_t century;
	uint16_t boot_architecture_flags;
	uint8_t reserved2;
	uint32_t flags;
	generic_address_structure_t reset_reg;
	uint8_t reset_value;
	uint8_t reserved3[3];
	uint64_t X_firmware_control;
	uint64_t X_dsdt;
	generic_address_structure_t X_PM1a_event_block;
	generic_address_structure_t X_PM1b_event_block;
	generic_address_structure_t X_PM1a_control_block;
	generic_address_structure_t X_PM1b_control_block;
	generic_address_structure_t X_PM2_control_block;
	generic_address_structure_t X_PM_timer_block;
	generic_address_structure_t X_GPE0_block;
	generic_address_structure_t X_GPE1_block;
} __attribute__((packed)) fadt_table_t;