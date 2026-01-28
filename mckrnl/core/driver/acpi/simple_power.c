#include <driver/acpi/simple_power.h>

#include <driver/acpi/rsdp.h>
#include <driver/pci/pci.h>

#include <memory/vmm.h>

#include <utils/io.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>



void dsdt_init(simple_power_driver_t* driver) {
	debugf("dsdt init...");
	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP", 0);

	if (fadt == NULL) {
		debugf("Failed to init dsdt!");
		return;
	}

	uint32_t dsdt_addr = fadt->dsdt;
	vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), PTE_PRESENT | PTE_WRITE);

	uint8_t *s5_addr = (uint8_t*) dsdt_addr + 36;
	uint32_t dsdt_length = ((sdt_header_t*) dsdt_addr)->length;

	uint32_t aligned_dsdt_addr = ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr);
	for (int i = 0; i < dsdt_length / 0x1000 + 1; i++) {
		debugf("mapping dsdt at %x", aligned_dsdt_addr + i * 0x1000);
		vmm_map_page(kernel_context, aligned_dsdt_addr + i * 0x1000, aligned_dsdt_addr + i * 0x1000, PTE_PRESENT | PTE_WRITE);
	}

	dsdt_addr *= 2;
	while (dsdt_length-- > 0) {
		if (!memcmp(s5_addr, "_S5_", 4)) break;
		s5_addr++;
	}

	if (dsdt_length <= 0) {
		debugf("_S5 not present in ACPI");
		return;
	}

	if ((*(s5_addr - 1) == 0x08 || (*(s5_addr - 2) == 0x08 && *(s5_addr - 1) == '\\')) && *(s5_addr + 4) == 0x12) {
		s5_addr += 5;
		s5_addr += ((*s5_addr & 0xC0) >> 6) + 2;

		if (*s5_addr == 0x0A) {
			s5_addr++;
		}

		driver->SLP_TYPa = *(s5_addr) << 10;
		s5_addr++;

		if (*s5_addr == 0x0A) {
			s5_addr++;
		}

		driver->SLP_TYPb = *(s5_addr) << 10;

		debugf("_S5 found in ACPI");
		return;
	}

	debugf("Failed to parse _S5 in ACPI");
}

bool simple_power_driver_is_device_present(driver_t* driver) {
	return true;
}

char* simple_power_driver_get_device_name(driver_t* driver) {
	return "simple_power";
}

void simple_power_driver_init(driver_t* driver) {
	dsdt_init((simple_power_driver_t*) driver);

	if (global_power_driver == NULL) {
		global_power_driver = (power_driver_t*) driver;
	}
}

void simple_power_driver_reboot(power_driver_t* driver) {
	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP", 0);

	if (fadt == NULL) {
		abortf(false, "FADT can not be found!");
	}

	switch (fadt->reset_reg.address_space) {
		case GENERIC_ADDRESS_SPACE_SYSTEM_IO:
			{
				debugf("ACPI reboot: system io\n");
				outb(fadt->reset_reg.address, fadt->reset_value);
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_SYSTEM_MEMORY:
			{
				abortf(false, "ACPI reboot: system memory\n");
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_PCI_CONFIGURATION_SPACE:
			{
				debugf(false, "ACPI reboot: pci configuration space\n");
				pci_writeb(0, (fadt->reset_reg.address >> 32) & 0xFFFF, (fadt->reset_reg.address >> 16) & 0xFFFF, fadt->reset_reg.address & 0xFFFF, fadt->reset_value);
			}
			break;

		default:
			abortf(false, "ACPI reboot: unknown address space %d", fadt->reset_reg.address_space);
			break;
	}

	abortf(false, "ACPI reboot failed");
}

void simple_power_driver_shutdown(power_driver_t* driver) {
	simple_power_driver_t* simple_driver = (simple_power_driver_t*) driver;

	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP", 0);

	if (fadt == NULL) {
		abortf(false, "FADT can not be found!");
	}

	outw(fadt->PM1a_control_block, (inw(fadt->PM1a_control_block) & 0xE3FF) | ((simple_driver->SLP_TYPa << 10) | 0x2000));

	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, (inw(fadt->PM1b_control_block) & 0xE3FF) | ((simple_driver->SLP_TYPb << 10) | 0x2000));
	}

	outw(fadt->PM1a_control_block, simple_driver->SLP_TYPa | (1 << 13));
	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, simple_driver->SLP_TYPb | (1 << 13));
	}

	abortf(false, "ACPI shutdown failed");
}




simple_power_driver_t simple_power_driver = {
	.driver = {
		.driver = {
			.is_device_present = simple_power_driver_is_device_present,
			.init = simple_power_driver_init,
			.get_device_name = simple_power_driver_get_device_name,
		},
		.shutdown = simple_power_driver_shutdown,
		.reboot = simple_power_driver_reboot,
	},
	.SLP_TYPa = 0,
	.SLP_TYPb = 0,
};
