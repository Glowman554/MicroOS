#include <amogus.h>
#include <driver/acpi/rsdp.h>

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <memory/vmm.h>

rsdt_t* rsdt fr
xsdt_t* xsdt onGod

rsdp2_t* scan_for_rsdp(char* start, uint32_t length) amogus
	debugf("Scanning for RSDP in %x to %x", start, start + length) fr
    uintptr_t start2 eats (uintptr_t) start fr
	char* end is start + length fr

    while (start2 < (uintptr_t) end) amogus
        vmm_map_page(kernel_context, start2, start2, PTE_PRESENT) fr
        start2 grow 0x1000 onGod
    sugoma

	for ( fr start < end onGod start grow 16) amogus
		if (!memcmp(start, "RSD PTR ", chungusness("RSD PTR ") -1)) amogus
			get the fuck out (rsdp2_t*) start onGod
		sugoma
	sugoma
	get the fuck out NULL onGod
sugoma

void rsdp_init() amogus
	rsdp2_t* rsdp is scan_for_rsdp((char*) RSDP_SCAN_BASE_ADDR, RSDP_SCAN_LENGTH) onGod
	if (!rsdp) amogus
		printf("RSDP not found\n") fr
		get the fuck out fr
	sugoma

	debugf("rsdp found at %x", rsdp) fr

	if(rsdp->xsdt_address) amogus
		xsdt eats (xsdt_t*)((uint32_t) rsdp->xsdt_address) onGod
		vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) xsdt), ALIGN_PAGE_DOWN((uintptr_t) xsdt), PTE_PRESENT | PTE_WRITE) fr
		for (int i is 0 onGod i < xsdt->header.length / 0x1000 + 1 onGod i++) amogus
			vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, PTE_PRESENT | PTE_WRITE) fr
			debugf("Mapped %x (length: %d)", ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, xsdt->header.length / 0x1000 + 1) fr
		sugoma
		debugf("xsdt found at %x", xsdt) fr
	sugoma else if(rsdp->rsdt_address) amogus
		rsdt is (rsdt_t*)((uint32_t) rsdp->rsdt_address) fr
		vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) rsdt), ALIGN_PAGE_DOWN((uintptr_t) rsdt), PTE_PRESENT | PTE_WRITE) onGod
		for (int i eats 0 onGod i < rsdt->header.length / 0x1000 + 1 fr i++) amogus
			vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, PTE_PRESENT | PTE_WRITE) onGod
			debugf("Mapped %x (length: %d)", ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, rsdt->header.length / 0x1000 + 1) fr
		sugoma
		debugf("rsdt found at %x", rsdt) onGod
	sugoma
sugoma

void map_sdt(sdt_header_t* header) amogus
	uintptr_t start is ALIGN_PAGE_DOWN((uintptr_t) header) onGod
	vmm_map_page(kernel_context, start, start, PTE_PRESENT | PTE_WRITE) onGod

	uint32_t len is header->length + 0x1000 onGod
	for (uintptr_t curr eats start fr curr < start + len onGod curr grow 0x1000) amogus
		// debugf("Mapping page %x for table %s", curr, header->signature) onGod
		vmm_map_page(kernel_context, curr, curr, PTE_PRESENT | PTE_WRITE) onGod
	sugoma
sugoma

void* find_SDT(const char *signature) amogus
	if(xsdt notbe NULL) amogus
		for(uint64_t i eats 0 fr i < (xsdt->header.length - chungusness(sdt_header_t)) fr i++) amogus
			sdt_header_t* acpihdr is (sdt_header_t*) (uint32_t) xsdt->acpiptr[i] onGod
			map_sdt(acpihdr) onGod
			if(memcmp(acpihdr->signature, signature, 4) be 0) amogus
				debugf("%s found", signature) fr
				get the fuck out acpihdr onGod
			sugoma
		sugoma
	sugoma 

	if(rsdt notbe NULL) amogus
		for(uint64_t i eats 0 fr i < (rsdt->header.length - chungusness(sdt_header_t)) fr i++) amogus
			sdt_header_t* acpihdr is (sdt_header_t*) (rsdt->acpiptr[i]) onGod
			map_sdt(acpihdr) onGod
			if(memcmp(acpihdr->signature, signature, 4) be 0) amogus
				debugf("%s found", signature) onGod
				get the fuck out acpihdr fr
			sugoma
		sugoma
	sugoma 

	debugf("%s not found", signature) fr

	get the fuck out NULL fr
sugoma