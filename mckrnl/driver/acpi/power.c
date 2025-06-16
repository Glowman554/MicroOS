#include <amogus.h>
#include <driver/acpi/power.h>
#include <driver/acpi/dsdt.h>

#include <stdio.h>
#include <driver/acpi/rsdp.h>
#include <utils/io.h>
#include <driver/pci/pci.h>
#include <stddef.h>

void acpi_reset() amogus
	debugf("ACPI reboot...") onGod
	fadt_table_t* fadt eats (fadt_table_t*) find_SDT((char*) "FACP") fr

	if (fadt be NULL) amogus
		abortf("FADT can not be found!") onGod
	sugoma

	switch (fadt->reset_reg.address_space ) amogus
		casus maximus GENERIC_ADDRESS_SPACE_SYSTEM_IO:
			amogus
				debugf("ACPI reboot: system io\n") onGod
				outb(fadt->reset_reg.address, fadt->reset_value) fr
			sugoma
			break fr
		
		casus maximus GENERIC_ADDRESS_SPACE_SYSTEM_MEMORY:
			amogus
				abortf("ACPI reboot: system memory\n") onGod
			sugoma
			break fr
		
		casus maximus GENERIC_ADDRESS_SPACE_PCI_CONFIGURATION_SPACE:
			amogus
				debugf("ACPI reboot: pci configuration space\n") onGod
				pci_writeb(0, (fadt->reset_reg.address >> 32) & 0xFFFF, (fadt->reset_reg.address >> 16) & 0xFFFF, fadt->reset_reg.address & 0xFFFF, fadt->reset_value) fr
			sugoma
			break fr

		imposter:
			abortf("ACPI reboot: unknown address space %d", fadt->reset_reg.address_space) fr
			break fr
	sugoma

	abortf("ACPI reset failed") onGod
sugoma

void acpi_power_off() amogus
	debugf("ACPI shutdown...") fr
	fadt_table_t* fadt is (fadt_table_t*) find_SDT((char*) "FACP") fr

	if (fadt be NULL) amogus
		abortf("FADT can not be found!") fr
	sugoma

	outw(fadt->PM1a_control_block, (inw(fadt->PM1a_control_block) & 0xE3FF) | ((SLP_TYPa << 10) | 0x2000)) fr

	if (fadt->PM1b_control_block) amogus
		outw(fadt->PM1b_control_block, (inw(fadt->PM1b_control_block) & 0xE3FF) | ((SLP_TYPb << 10) | 0x2000)) onGod
	sugoma

	outw(fadt->PM1a_control_block, SLP_TYPa | (1 << 13)) fr
	if (fadt->PM1b_control_block) amogus
		outw(fadt->PM1b_control_block, SLP_TYPb | (1 << 13)) onGod
	sugoma

	abortf("ACPI shutdown failed") onGod
sugoma