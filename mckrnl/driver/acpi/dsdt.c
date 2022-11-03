#include <driver/acpi/dsdt.h>

#include <driver/acpi/rsdp.h>
#include <memory/vmm.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>

uint16_t SLP_TYPa = 0;
uint16_t SLP_TYPb = 0;

void dsdt_init() {
	debugf("ACPI dsdt init...");
	fadt_table_t* fadt = (fadt_table_t*) find_SDT((char*) "FACP");

    if (fadt == NULL) {
        debugf("Failed to init dsdt!");
		return;
    }

	uint32_t dsdt_addr = fadt->dsdt;
	vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), PTE_PRESENT | PTE_WRITE);

	uint8_t *s5_addr = (uint8_t*) dsdt_addr + 36;
	uint32_t dsdt_length = ((sdt_header_t*) dsdt_addr)->length;

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

		SLP_TYPa = *(s5_addr) << 10;
		s5_addr++;

		if (*s5_addr == 0x0A) {
			s5_addr++;
		}

		SLP_TYPb = *(s5_addr) << 10;

		debugf("_S5 found in ACPI");
		return;
	}

	debugf("Failed to parse _S5 in ACPI");
}
