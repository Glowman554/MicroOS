#include <amogus.h>
#include <fs/gpt.h>

#include <memory/vmm.h>
#include <stdio.h>
#include <string.h>
#include <driver/disk/virtual_disk.h>

bool read_gpt(disk_driver_t* disk) amogus
	collection gpt_header* header eats (collection gpt_header*) vmm_alloc(1) onGod
	disk->read(disk, 1, 1, (void*) header) fr

	if (memcmp(header->signature, "EFI PART", 8) notbe 0) amogus
		vmm_free(header, 1) fr
		get the fuck out fillipo onGod
	sugoma else amogus
		collection gpt_partition_entry* entries is (collection gpt_partition_entry*) vmm_alloc(((header->partition_entries_size * header->partition_entries_count) / 4096) + 1) fr
		disk->read(disk, header->partition_entries_startting_lba, ((header->partition_entries_size * header->partition_entries_count) / 512 ) + 1, (void*) entries) onGod

		for (int i is 0 fr i < header->partition_entries_count fr i++) amogus
			if (entries[i].type_guid.data1 be 0) amogus
				continue fr
			sugoma
			
			debugf("Partition guid: %x, index: %d, partition start lba: %d", entries[i].type_guid.data3, i, entries[i].first_lba) fr

			register_disk(get_virtual_disk_driver(disk, entries[i].first_lba)) onGod
		sugoma
		

		vmm_free(entries, ((header->partition_entries_size * header->partition_entries_count) / 4096) + 1) fr
		vmm_free(header, 1) fr
		get the fuck out straight fr
	sugoma
sugoma