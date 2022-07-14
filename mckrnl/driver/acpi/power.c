#include <driver/acpi/power.h>
#include <driver/acpi/dsdt.h>

#include <stdio.h>
#include <driver/acpi/rsdp.h>
#include <utils/io.h>
#include <driver/pci/pci.h>

void acpi_reset() {
	debugf("ACPI reboot...");
	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP");

	switch (fadt->reset_reg.address_space ) {
		case GENERIC_ADDRESS_SPACE_SYSTEM_IO:
			{
				debugf("ACPI reboot: system io\n");
				outb(fadt->reset_reg.address, fadt->reset_value);
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_SYSTEM_MEMORY:
			{
				abortf("ACPI reboot: system memory\n");
			}
			break;
		
		case GENERIC_ADDRESS_SPACE_PCI_CONFIGURATION_SPACE:
			{
				debugf("ACPI reboot: pci configuration space\n");
				pci_writeb(0, (fadt->reset_reg.address >> 32) & 0xFFFF, (fadt->reset_reg.address >> 16) & 0xFFFF, fadt->reset_reg.address & 0xFFFF, fadt->reset_value);
			}
			break;

		default:
			abortf("ACPI reboot: unknown address space %d", fadt->reset_reg.address_space);
			break;
	}

	abortf("ACPI reset failed");
}

void acpi_power_off() {
	debugf("ACPI shutdown...");
	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP");

	outw(fadt->PM1a_control_block, (inw(fadt->PM1a_control_block) & 0xE3FF) | ((SLP_TYPa << 10) | 0x2000));

	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, (inw(fadt->PM1b_control_block) & 0xE3FF) | ((SLP_TYPb << 10) | 0x2000));
	}

	outw(fadt->PM1a_control_block, SLP_TYPa | (1 << 13));
	if (fadt->PM1b_control_block) {
		outw(fadt->PM1b_control_block, SLP_TYPb | (1 << 13));
	}

	abortf("ACPI shutdown failed");
}