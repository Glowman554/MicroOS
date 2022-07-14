#include <driver/pci/pci.h>

#include <stdio.h>

void enumerate_pci() {
	for (uint16_t bus = 0; bus < 8; bus++) {
		for (uint16_t device = 0; device < 32; device++) {
			int num_functions = device_has_functions(bus, device) ? 8 : 1;
			for (uint64_t function = 0; function < num_functions; function++) {
				pci_device_header_t pci_header = get_device_header(bus, device, function);
				if(pci_header.vendor_id == 0x0000 || pci_header.vendor_id == 0xffff) {
					continue;
				}

				debugf("Vendor name: %s, Device name: %s, Device class: %s, Sub class name: %s, Prog interface name: %s",  get_device_name(pci_header.vendor_id, pci_header.device_id), device_classes[pci_header.class_], get_subclass_name(pci_header.class_, pci_header.subclass), get_prog_IF_name(pci_header.class_, pci_header.subclass, pci_header.prog_if));
			}
		}
	}
}