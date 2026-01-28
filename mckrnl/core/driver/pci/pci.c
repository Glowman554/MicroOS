#include <driver/pci/pci.h>

#include <stdio.h>
#include <config.h>

#include <stddef.h>
#include <memory/heap.h>

pci_driver_t* pci_drivers = NULL;
int num_pci_drivers = 0;

pci_device_list_entry_t* pci_devices = NULL;
int num_pci_devices = 0;

void apend_pci_device(pci_device_header_t header,  uint16_t bus, uint16_t device, uint16_t function, bool driver) {
	pci_device_t devices = {
		.header = header,
		.bus = bus,
		.device = device,
		.function = function
	};

	pci_device_list_entry_t entry = {
		.device = devices,
		.driver_loaded = driver
	};

	pci_devices = krealloc(pci_devices, sizeof(pci_device_list_entry_t) * (num_pci_devices + 1));
	pci_devices[num_pci_devices++] = entry;
}

void enumerate_pci() {
	for (uint16_t bus = 0; bus < 8; bus++) {
		for (uint16_t device = 0; device < 32; device++) {
			int num_functions = device_has_functions(bus, device) ? 8 : 1;
			for (uint64_t function = 0; function < num_functions; function++) {
				pci_device_header_t pci_header = get_device_header(bus, device, function);
				if(pci_header.vendor_id == 0x0000 || pci_header.vendor_id == 0xffff) {
					continue;
				}

				bool driver = false;

				for (int i = 0; i < num_pci_drivers; i++) {
					if (pci_drivers[i].use_class_subclass_prog_IF) {
						if ((pci_drivers[i]._class == pci_header.class_ || pci_drivers[i]._class == 0) && (pci_drivers[i].subclass == pci_header.subclass || pci_drivers[i].subclass == 0) && (pci_drivers[i].prog_IF == pci_header.prog_if || pci_drivers[i].prog_IF == 0)) {
							debugf("PCI: Found driver for %x:%x:%x: %p", bus, device, function, pci_drivers[i].load_driver);
							pci_drivers[i].load_driver(pci_header, bus, device, function);
							driver = true;
						}
					} else if (pci_drivers[i].use_vendor_device_id) {
						if (pci_drivers[i].vendor_id == pci_header.vendor_id && pci_drivers[i].device_id == pci_header.device_id) {
							debugf("PCI: Found driver for %x:%x:%x: %p", bus, device, function, pci_drivers[i].load_driver);
							pci_drivers[i].load_driver(pci_header, bus, device, function);
							driver = true;
						}
					} else {
						abortf(false, "Invalid pci driver");
					}
				}

				apend_pci_device(pci_header, bus, device, function, driver);
				debugf("Vendor name: %s, Device name: %s, Device class: %s, Sub class name: %s, Prog interface name: %s", get_vendor_name(pci_header.vendor_id), get_device_name(pci_header.vendor_id, pci_header.device_id), get_device_class(pci_header.class_), get_subclass_name(pci_header.class_, pci_header.subclass), get_prog_IF_name(pci_header.class_, pci_header.subclass, pci_header.prog_if));
			}
		}
	}
	breakpoint();
}

void register_pci_driver_cs(uint8_t _class, uint8_t subclass, uint8_t prog_IF, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function)) {
	pci_driver_t driver = { 0 };
	driver._class = _class;
	driver.subclass = subclass;
	driver.prog_IF = prog_IF;
	driver.use_class_subclass_prog_IF = true;
	driver.load_driver = load_driver;
	pci_drivers = krealloc(pci_drivers, sizeof(pci_driver_t) * (num_pci_drivers + 1));
	pci_drivers[num_pci_drivers++] = driver;

	debugf("Registered PCI driver with class %d, subclass %d, prog_IF %d", _class, subclass, prog_IF);
}

void register_pci_driver_vd(uint16_t vendor_id, uint16_t device_id, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function)) {
	pci_driver_t driver = { 0 };
	driver.vendor_id = vendor_id;
	driver.device_id = device_id;
	driver.use_vendor_device_id = true;
	driver.load_driver = load_driver;
	pci_drivers = krealloc(pci_drivers, sizeof(pci_driver_t) * (num_pci_drivers + 1));
	pci_drivers[num_pci_drivers++] = driver;

	debugf("Registered PCI driver with vendor_id %x, device_id %x", vendor_id, device_id);
}