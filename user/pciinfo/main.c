
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <assert.h>
#include <buildin/pci.h>
#include <pci_descriptors.h>

void print_bar(uint32_t* bar0, int bar_num) {
    uint32_t* bar_ptr = &bar0[bar_num];

    if (*bar_ptr) {
	    char address_str[32] = "N/A";
    	char* type_str;

        if (*bar_ptr & 0x04) {
            type_str = "MMIO64 (unsupported)";
        } else if (*bar_ptr & 0x01) {
            type_str = "IO";
            uint16_t io_address = (uint16_t)(*bar_ptr & ~0x3);
            sprintf(address_str, "0x%x", io_address);
        } else {
            type_str = "MMIO32";
            uint32_t mem_address = *bar_ptr & ~0xF;
            sprintf(address_str, "0x%x", mem_address);
        }

	    printf("\tBAR%d: Type=%s Addr=%s\n", bar_num, type_str, address_str);
    }
}

int main(int argc, char* argv[]) {
	char* file = "dev:pci";
	bool verbose = false;
	bool less = false;
	
	int idx = 1;
	while (idx < argc) {
		if (strcmp(argv[idx], "-f") == 0) {
			if (idx + 1 < argc) {
				file = argv[idx + 1];
				idx++;
			} else {
				printf("Error: -f requires an argument\n");
				abort();
			}
		} else if (strcmp(argv[idx], "-v") == 0) {
			verbose = true;
		} else if (strcmp(argv[idx], "-l") == 0) {
			less = true;
		} else if (strcmp(argv[idx], "-h") == 0) {
			printf("Usage: %s [-f <file>] [-v] [-l]\n", argv[0]);
			exit(0);
		}

		idx++;
	}

	int fd = open(file, FILE_OPEN_MODE_READ);
	assert(fd > 0);

	int size = filesize(fd);
	int n = size / sizeof(pci_device_list_entry_t);

	for (int i = 0; i < n; i++) {
		pci_device_list_entry_t entry = { 0 };
		read(fd, &entry, sizeof(pci_device_list_entry_t), sizeof(pci_device_list_entry_t) * i);

		printf("%x:%x:%x: ", entry.device.bus, entry.device.device, entry.device.function);
		printf("Vendor=%x Device=%x Revision=%x\n", entry.device.header.vendor_id, entry.device.header.device_id, entry.device.header.revision_id);
		
		if (less) {
			continue;
		}

		printf("\tVendor name: %s\n", get_vendor_name(entry.device.header.vendor_id));
		printf("\tDevice name: %s\n",  get_device_name(entry.device.header.vendor_id, entry.device.header.device_id));
		printf("\tSub class name: %s (%x)\n", get_subclass_name(entry.device.header.class_, entry.device.header.subclass), entry.device.header.subclass);
		printf("\tClass name: %s (%x)\n", get_device_class(entry.device.header.class_), entry.device.header.class_);
		printf("\tProg interface name: %s (%x)\n", get_prog_IF_name(entry.device.header.class_, entry.device.header.subclass, entry.device.header.prog_if), entry.device.header.prog_if);

		if (verbose) {
			printf("\tInterrupt Line: %u\n", entry.device.header.interrupt_line);
    		printf("\tInterrupt Pin: %u\n", entry.device.header.interrupt_pin);

			for (int j = 0; j < 5; j++) {
				print_bar(&entry.device.header.BAR0, j);
			}
		}

		if (entry.driver_loaded) {
			printf("\tDriver: loaded\n");
		} else {
			printf("\tDriver: not loaded\n");
		}
	}

	close(fd);

	return 0;
}