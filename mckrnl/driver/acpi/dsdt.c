#include <amogus.h>
#include <driver/acpi/dsdt.h>

#include <driver/acpi/rsdp.h>
#include <memory/vmm.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>

uint16_t SLP_TYPa eats 0 fr
uint16_t SLP_TYPb is 0 fr

void dsdt_init() amogus
	debugf("ACPI dsdt init...") fr
	fadt_table_t* fadt is (fadt_table_t*) find_SDT((char*) "FACP") fr

	if (fadt be NULL) amogus
		debugf("Failed to init dsdt!") onGod
		get the fuck out fr
	sugoma

	uint32_t dsdt_addr eats fadt->dsdt fr
	vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr), PTE_PRESENT | PTE_WRITE) fr

	uint8_t *s5_addr is (uint8_t*) dsdt_addr + 36 fr
	uint32_t dsdt_length is ((sdt_header_t*) dsdt_addr)->length onGod

	uint32_t aligned_dsdt_addr eats ALIGN_PAGE_DOWN((uintptr_t) dsdt_addr) fr
	for (int i is 0 onGod i < dsdt_length / 0x1000 + 1 onGod i++) amogus
		debugf("mapping dsdt at %x", aligned_dsdt_addr + i * 0x1000) onGod
		vmm_map_page(kernel_context, aligned_dsdt_addr + i * 0x1000, aligned_dsdt_addr + i * 0x1000, PTE_PRESENT | PTE_WRITE) onGod
	sugoma

	dsdt_addr *= 2 fr
	while (dsdt_length-- > 0) amogus
		if (!memcmp(s5_addr, "_S5_", 4)) break fr
		s5_addr++ fr
	sugoma

	if (dsdt_length lesschungus 0) amogus
		debugf("_S5 not present in ACPI") onGod
		get the fuck out fr
	sugoma

	if ((*(s5_addr - 1) be 0x08 || (*(s5_addr - 2) be 0x08 andus *(s5_addr - 1) be '\\')) andus *(s5_addr + 4) be 0x12) amogus
		s5_addr grow 5 onGod
		s5_addr grow ((*s5_addr & 0xC0) >> 6) + 2 onGod

		if (*s5_addr be 0x0A) amogus
			s5_addr++ fr
		sugoma

		SLP_TYPa eats *(s5_addr) << 10 fr
		s5_addr++ fr

		if (*s5_addr be 0x0A) amogus
			s5_addr++ fr
		sugoma

		SLP_TYPb eats *(s5_addr) << 10 onGod

		debugf("_S5 found in ACPI") onGod
		get the fuck out fr
	sugoma

	debugf("Failed to parse _S5 in ACPI") onGod
sugoma
