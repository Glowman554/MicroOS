#include <driver/acpi/rsdp.h>

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <memory/vmm.h>

rsdt_t* rsdt;
xsdt_t* xsdt;

rsdp2_t* scan_for_rsdp(char* start, uint32_t length) {
	debugf("Scanning for RSDP in %x to %x", start, start + length);
	char* end = start + length;

	for (; start < end; start += 16) {
		if (!memcmp(start, "RSD PTR ", sizeof("RSD PTR ") -1)) {
			return (rsdp2_t*) start;
		}
	}
	return NULL;
}

void rsdp_init() {
	rsdp2_t* rsdp = scan_for_rsdp((char*) RSDP_SCAN_BASE_ADDR, RSDP_SCAN_LENGTH);
	if (!rsdp) {
		printf("RSDP not found\n");
		return;
	}

	debugf("rsdp found at %x", rsdp);

	if(rsdp->xsdt_address) {
		xsdt = (xsdt_t*)((uint32_t) rsdp->xsdt_address);
		vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) xsdt), ALIGN_PAGE_DOWN((uintptr_t) xsdt), PTE_PRESENT | PTE_WRITE);
		for (int i = 0; i < xsdt->header.length / 0x1000 + 1; i++) {
			vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, PTE_PRESENT | PTE_WRITE);
			debugf("Mapped %x (length: %d)", ALIGN_PAGE_DOWN((uintptr_t) xsdt) + i * 0x1000, xsdt->header.length / 0x1000 + 1);
		}
		debugf("xsdt found at %x", xsdt);
	} else if(rsdp->rsdt_address) {
		rsdt = (rsdt_t*)((uint32_t) rsdp->rsdt_address);
		vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) rsdt), ALIGN_PAGE_DOWN((uintptr_t) rsdt), PTE_PRESENT | PTE_WRITE);
		for (int i = 0; i < rsdt->header.length / 0x1000 + 1; i++) {
			vmm_map_page(kernel_context, ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, PTE_PRESENT | PTE_WRITE);
			debugf("Mapped %x (length: %d)", ALIGN_PAGE_DOWN((uintptr_t) rsdt) + i * 0x1000, rsdt->header.length / 0x1000 + 1);
		}
		debugf("rsdt found at %x", rsdt);
	}
}

void* find_SDT(const char *signature) {
	if(xsdt != NULL) {
		for(uint64_t i = 0; i < (xsdt->header.length - sizeof(sdt_header_t)); i++) {
			sdt_header_t* acpihdr = (sdt_header_t*) (uint32_t) xsdt->acpiptr[i];
			if(memcmp(acpihdr->signature, signature, 4) == 0) {
				debugf("%s found", signature);
				return acpihdr;
			}
		}
	} 

	if(rsdt != NULL) {
		for(uint64_t i = 0; i < (rsdt->header.length - sizeof(sdt_header_t)); i++) {
			sdt_header_t* acpihdr = (sdt_header_t*) (rsdt->acpiptr[i]);
			if(memcmp(acpihdr->signature, signature, 4) == 0) {
				debugf("%s found", signature);
				return acpihdr;
			}
		}
	} 

	debugf("%s not found", signature);

	return NULL;
}