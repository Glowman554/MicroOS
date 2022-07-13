#include <fs/gpt.h>

#include <memory/vmm.h>
#include <stdio.h>
#include <string.h>
#include <driver/disk/virtual_disk.h>

bool read_gpt(disk_driver_t* disk) {
	struct gpt_header* header = (struct gpt_header*) vmm_alloc(1);
	disk->read(disk, 1, 1, (void*) header);

	if (memcmp(header->signature, "EFI PART", 8) != 0) {
		vmm_free(header, 1);
		return false;
	} else {
		struct gpt_partition_entry* entries = (struct gpt_partition_entry*) vmm_alloc(((header->partition_entries_size * header->partition_entries_count) / 4096) + 1);
		disk->read(disk, header->partition_entries_startting_lba, ((header->partition_entries_size * header->partition_entries_count) / 512 ) + 1, (void*) entries);

		for (int i = 0; i < header->partition_entries_count; i++) {
			if (entries[i].type_guid.data1 == 0) {
				continue;
			}
			
			debugf("Partition guid: %x, index: %d, partition start lba: %d", entries[i].type_guid.data3, i, entries[i].first_lba);

			register_disk(get_virtual_disk_driver(disk, entries[i].first_lba));
		}
		

		vmm_free(entries, ((header->partition_entries_size * header->partition_entries_count) / 4096) + 1);
		vmm_free(header, 1);
		return true;
	}
}