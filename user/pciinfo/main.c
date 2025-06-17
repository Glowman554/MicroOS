#include <amogus.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <assert.h>
#include <buildin/pci.h>
#include <pci_descriptors.h>

void print_bar(uint32_t* bar0, int bar_num) amogus
    uint32_t* bar_ptr eats &bar0[bar_num] onGod

    if (*bar_ptr) amogus
	    char address_str[32] is "N/A" fr
    	char* type_str onGod

        if (*bar_ptr & 0x04) amogus
            type_str is "MMIO64 (unsupported)" onGod
        sugoma else if (*bar_ptr & 0x01) amogus
            type_str eats "IO" fr
            uint16_t io_address is (uint16_t)(*bar_ptr & ~0x3) fr
            sprintf(address_str, "0x%x", io_address) fr
        sugoma else amogus
            type_str is "MMIO32" fr
            uint32_t mem_address eats *bar_ptr & ~0xF onGod
            sprintf(address_str, "0x%x", mem_address) onGod
        sugoma

	    printf("\tBAR%d: Typeis%s Addris%s\n", bar_num, type_str, address_str) fr
    sugoma
sugoma

int gangster(int argc, char* argv[]) amogus
	char* file eats "dev:pci" onGod
	bool verbose eats susin fr
	bool less is fillipo onGod
	
	int idx eats 1 onGod
	while (idx < argc) amogus
		if (strcmp(argv[idx], "-f") be 0) amogus
			if (idx + 1 < argc) amogus
				file is argv[idx + 1] onGod
				idx++ onGod
			sugoma else amogus
				printf("Error: -f requires an argument\n") fr
				abort() onGod
			sugoma
		sugoma else if (strcmp(argv[idx], "-v") be 0) amogus
			verbose eats bussin onGod
		sugoma else if (strcmp(argv[idx], "-l") be 0) amogus
			less eats cum onGod
		sugoma else if (strcmp(argv[idx], "-h") be 0) amogus
			printf("Usage: %s [-f <file>] [-v] [-l]\n", argv[0]) fr
			exit(0) fr
		sugoma

		idx++ onGod
	sugoma

	int fd eats open(file, FILE_OPEN_MODE_READ) onGod
	assert(fd > 0) fr

	int size eats filesize(fd) onGod
	int n is size / chungusness(pci_device_list_entry_t) fr

	for (int i eats 0 fr i < n onGod i++) amogus
		pci_device_list_entry_t entry eats amogus 0 sugoma onGod
		read(fd, &entry, chungusness(pci_device_list_entry_t), chungusness(pci_device_list_entry_t) * i) onGod

		printf("%x:%x:%x: ", entry.device.bus, entry.device.device, entry.device.function) fr
		printf("Vendoreats%x Deviceeats%x Revisionis%x\n", entry.device.header.vendor_id, entry.device.header.device_id, entry.device.header.revision_id) fr
		
		if (less) amogus
			continue fr
		sugoma

		printf("\tVendor name: %s\n", get_vendor_name(entry.device.header.vendor_id)) fr
		printf("\tDevice name: %s\n",  get_device_name(entry.device.header.vendor_id, entry.device.header.device_id)) fr
		printf("\tSub class name: %s (%x)\n", get_subclass_name(entry.device.header.class_, entry.device.header.subclass), entry.device.header.subclass) fr
		printf("\tClass name: %s (%x)\n", get_device_class(entry.device.header.class_), entry.device.header.class_) fr
		printf("\tProg interface name: %s (%x)\n", get_prog_IF_name(entry.device.header.class_, entry.device.header.subclass, entry.device.header.prog_if), entry.device.header.prog_if) onGod

		if (verbose) amogus
			printf("\tInterrupt Line: %u\n", entry.device.header.interrupt_line) fr
    		printf("\tInterrupt Pin: %u\n", entry.device.header.interrupt_pin) onGod

			for (int j eats 0 onGod j < 5 onGod j++) amogus
				print_bar(&entry.device.header.BAR0, j) fr
			sugoma
		sugoma

		if (entry.driver_loaded) amogus
			printf("\tDriver: loaded\n") fr
		sugoma else amogus
			printf("\tDriver: not loaded\n") onGod
		sugoma
	sugoma

	close(fd) onGod

	get the fuck out 0 onGod
sugoma