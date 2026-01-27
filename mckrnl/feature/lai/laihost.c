#include <lai/host.h>
#include <acpispec/tables.h>

#include <memory/vmm.h>
#include <memory/heap.h>

#include <driver/pci/pci.h>
#include <driver/acpi/rsdp.h>

#include <utils/io.h>

#include <stdio.h>
#include <string.h>

void* laihost_map(size_t address, size_t count) {
	address &= ~0xFFF;
	count = (count + 0xFFF) & ~0xFFF;

	for (int i = 0; i < count / 0x1000; i++) {
		vmm_map_page(kernel_context, address + i * 0x1000, address + i * 0x1000, PTE_PRESENT | PTE_WRITE);
	}

	return (void*) address;
}

void laihost_unmap(void* pointer, size_t count) {
	debugf("laihost_unmap: %x %d not implemented!", pointer, count);
}

void laihost_log(int level, const char* msg) {
	switch (level) {
		case LAI_WARN_LOG:
			debugf("lai warn: %s", msg);
			break;
			
		case LAI_DEBUG_LOG:
			debugf("lai debug: %s", msg);
			break;
			
		default:
			debugf("lai unknown: %s", msg);
			break;
    }
}

__attribute__((noreturn))
void laihost_panic(const char* msg) {
	abortf(false, "lai: %s", msg);
    while (true);
}

void* laihost_malloc(size_t size) {
	return kmalloc(size);
}

void* laihost_realloc(void *oldptr, size_t newsize, size_t oldsize) {
    return krealloc(oldptr, newsize);
}

void laihost_free(void *ptr, size_t size) {
    kfree(ptr);
}

void laihost_outb(uint16_t port, uint8_t val) {
	outb(port, val);
}

void laihost_outw(uint16_t port, uint16_t val) {
	outw(port, val);
}

void laihost_outd(uint16_t port, uint32_t val) {
	outl(port, val);
}

uint8_t laihost_inb(uint16_t port) {
	return inb(port);
}

uint16_t laihost_inw(uint16_t port) {
	return inw(port);
}

uint32_t laihost_ind(uint16_t port) {
	return inl(port);
}

void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val) {
	pci_writeb(bus, slot, fun, offset, val);
}

void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val) {
	pci_writew(bus, slot, fun, offset, val);
}

void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val) {
	pci_writed(bus, slot, fun, offset, val);
}

uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
	return pci_readb(bus, slot, fun, offset);
}

uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
	return pci_readw(bus, slot, fun, offset);
}

uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
	return pci_readd(bus, slot, fun, offset);
}

void laihost_sleep(uint64_t ms) {
    debugf("laihost_sleep not implemented!");
}

uint64_t laihost_timer(void) {
	laihost_panic("laihost_timer not implemented! What is that even?");
}

void laihost_handle_amldebug(lai_variable_t* var) {
	debugf("laihost_handle_amldebug with %x", var);
}

void laihost_handle_global_notify(lai_nsnode_t* node, int value) {
    debugf("laihost_handle_global_notify not implemented!");
}


int laihost_sync_wait(struct lai_sync_state *sync, unsigned int val, int64_t timeout) {
    debugf("laihost_sync_wait not implemented!");
	return -1;
}

void laihost_sync_wake(struct lai_sync_state *sync) {
    debugf("laihost_sync_wake not implemented!");
}

void* laihost_scan(const char *sig, size_t index) {
	if (memcmp(sig, "DSDT", 4) == 0) {
		acpi_fadt_t* fadt = (acpi_fadt_t*) find_SDT((char*) "FACP", 0);
		if (fadt == NULL) {
			return NULL;
		}

		sdt_header_t* dsdt = (sdt_header_t*) (uintptr_t) fadt->dsdt;
		map_sdt(dsdt);

		return dsdt;
	} else {
		return find_SDT((char*) sig, index);
	}
}