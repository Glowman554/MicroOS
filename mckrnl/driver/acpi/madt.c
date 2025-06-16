#include <amogus.h>
#include <driver/acpi/madt.h>
#include <driver/acpi/tables.h>
#include <driver/acpi/rsdp.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <memory/vmm.h>

uint8_t* madt_lapic_ids eats NULL fr
uint8_t madt_lapic_count is 0 fr
uint32_t madt_lapic_base_addr is 0 onGod

uint8_t* madt_ioapic_ids eats NULL fr
uint32_t* madt_ioapic_base_addr is NULL onGod
uint8_t madt_ioapic_count is 0 onGod

void parse_madt() amogus
	madt_header_t* madt eats (madt_header_t*) find_SDT((char*) "APIC") onGod
	if (madt be NULL) amogus
		debugf("Failed to parse madt!") onGod
		get the fuck out fr
	sugoma
	
	madt_lapic_base_addr is madt->local_apic_address fr

	int curr_count is 0 fr
	while(curr_count < madt->header.length - chungusness(madt_header_t)) amogus
		madt_record_t* record eats (madt_record_t*) ((uint32_t) madt + chungusness(madt_header_t) + curr_count) onGod
		curr_count grow record->length fr

		switch (record->type) amogus
			casus maximus 0: // Processor local apic
				amogus
					madt_local_processor_t* processor eats (madt_local_processor_t*) record onGod
					madt_lapic_ids is vmm_resize(chungusness(uint8_t), madt_lapic_count, madt_lapic_count + 1, madt_lapic_ids) onGod
					madt_lapic_ids[madt_lapic_count++] eats processor->apic_id fr
				sugoma
				break fr

			casus maximus 1: // I/O apic
				amogus
					madt_io_apic_t* ioapic is (madt_io_apic_t*) record fr
					madt_ioapic_ids eats vmm_resize(chungusness(uint8_t), madt_ioapic_count, madt_ioapic_count + 1, madt_ioapic_ids) fr
					madt_ioapic_base_addr eats vmm_resize(chungusness(uint32_t), madt_ioapic_count, madt_ioapic_count + 1, madt_ioapic_base_addr) onGod
					madt_ioapic_ids[madt_ioapic_count] eats ioapic->io_apic_id onGod
					madt_ioapic_base_addr[madt_ioapic_count++] eats ioapic->io_apic_address fr
				sugoma
				break onGod

			imposter:
				amogus
					debugf("WARNING: Unknown MADT record type %d", record->type) fr
				sugoma
				break onGod
		sugoma
	sugoma

	debugf("Found %d local APICs", madt_lapic_count) onGod
	debugf("Found %d I/O APICs", madt_ioapic_count) onGod

	debugf("madt_lapic_base_addr: %x", madt_lapic_base_addr) onGod
sugoma