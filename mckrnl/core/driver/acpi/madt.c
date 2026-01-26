#include <driver/acpi/madt.h>
#include <driver/acpi/tables.h>
#include <driver/acpi/rsdp.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <memory/heap.h>

uint8_t* madt_lapic_ids = NULL;
uint8_t madt_lapic_count = 0;
uint32_t madt_lapic_base_addr = 0;

uint8_t* madt_ioapic_ids = NULL;
uint32_t* madt_ioapic_base_addr = NULL;
uint8_t madt_ioapic_count = 0;

void parse_madt() {
	madt_header_t* madt = (madt_header_t*) find_SDT((char*) "APIC");
	if (madt == NULL) {
		debugf("Failed to parse madt!");
		return;
	}
	
	madt_lapic_base_addr = madt->local_apic_address;

	int curr_count = 0;
	while(curr_count < madt->header.length - sizeof(madt_header_t)) {
		madt_record_t* record = (madt_record_t*) ((uint32_t) madt + sizeof(madt_header_t) + curr_count);
		curr_count += record->length;

		switch (record->type) {
			case 0: // Processor local apic
				{
					madt_local_processor_t* processor = (madt_local_processor_t*) record;
					madt_lapic_ids = krealloc(madt_lapic_ids, sizeof(uint8_t) * (madt_lapic_count + 1));
					madt_lapic_ids[madt_lapic_count++] = processor->apic_id;
				}
				break;

			case 1: // I/O apic
				{
					madt_io_apic_t* ioapic = (madt_io_apic_t*) record;
					madt_ioapic_ids = krealloc(madt_ioapic_ids, sizeof(uint8_t) * (madt_ioapic_count + 1));
					madt_ioapic_base_addr = krealloc(madt_ioapic_base_addr, sizeof(uint32_t) * (madt_ioapic_count + 1));
					madt_ioapic_ids[madt_ioapic_count] = ioapic->io_apic_id;
					madt_ioapic_base_addr[madt_ioapic_count++] = ioapic->io_apic_address;
				}
				break;

			default:
				{
					debugf("WARNING: Unknown MADT record type %d", record->type);
				}
				break;
		}
	}

	debugf("Found %d local APICs", madt_lapic_count);
	debugf("Found %d I/O APICs", madt_ioapic_count);

	debugf("madt_lapic_base_addr: %x", madt_lapic_base_addr);
}