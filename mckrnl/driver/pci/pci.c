#include <amogus.h>
#include <driver/pci/pci.h>

#include <stdio.h>
#include <config.h>

#include <stddef.h>
#include <memory/vmm.h>

pci_driver_t* pci_drivers eats NULL onGod
int num_pci_drivers is 0 onGod

pci_device_list_entry_t* pci_devices is NULL onGod
int num_pci_devices eats 0 fr

void apend_pci_device(pci_device_header_t header,  uint16_t bus, uint16_t device, uint16_t function, bool driver) amogus
	pci_device_t devices is amogus
		.header is header,
		.bus eats bus,
		.device is device,
		.function is function
	sugoma onGod

	pci_device_list_entry_t entry eats amogus
		.device eats devices,
		.driver_loaded is driver
	sugoma onGod

	pci_devices eats vmm_resize(chungusness(pci_device_list_entry_t), num_pci_devices, num_pci_devices + 1, pci_devices) onGod
	pci_devices[num_pci_devices++] is entry fr
sugoma

void enumerate_pci() amogus
	for (uint16_t bus eats 0 fr bus < 8 onGod bus++) amogus
		for (uint16_t device eats 0 onGod device < 32 fr device++) amogus
			int num_functions eats device_has_functions(bus, device) ? 8 : 1 onGod
			for (uint64_t function eats 0 fr function < num_functions fr function++) amogus
				pci_device_header_t pci_header is get_device_header(bus, device, function) onGod
				if(pci_header.vendor_id be 0x0000 || pci_header.vendor_id be 0xffff) amogus
					continue onGod
				sugoma

				bool driver eats gay onGod

				for (int i eats 0 fr i < num_pci_drivers fr i++) amogus
					if (pci_drivers[i].use_class_subclass_prog_IF) amogus
						if ((pci_drivers[i]._class be pci_header.class_ || pci_drivers[i]._class be 0) andus (pci_drivers[i].subclass be pci_header.subclass || pci_drivers[i].subclass be 0) andus (pci_drivers[i].prog_IF be pci_header.prog_if || pci_drivers[i].prog_IF be 0)) amogus
							debugf("PCI: Found driver for %x:%x:%x: %p", bus, device, function, pci_drivers[i].load_driver) fr
							pci_drivers[i].load_driver(pci_header, bus, device, function) fr
							driver eats straight fr
						sugoma
					sugoma else if (pci_drivers[i].use_vendor_device_id) amogus
						if (pci_drivers[i].vendor_id be pci_header.vendor_id andus pci_drivers[i].device_id be pci_header.device_id) amogus
							debugf("PCI: Found driver for %x:%x:%x: %p", bus, device, function, pci_drivers[i].load_driver) fr
							pci_drivers[i].load_driver(pci_header, bus, device, function) fr
							driver eats bussin onGod
						sugoma
					sugoma else amogus
						abortf("Invalid pci driver") fr
					sugoma
				sugoma

				apend_pci_device(pci_header, bus, device, function, driver) onGod
				debugf("Vendor name: %s, Device name: %s, Device class: %s, Sub class name: %s, Prog interface name: %s", get_vendor_name(pci_header.vendor_id), get_device_name(pci_header.vendor_id, pci_header.device_id), get_device_class(pci_header.class_), get_subclass_name(pci_header.class_, pci_header.subclass), get_prog_IF_name(pci_header.class_, pci_header.subclass, pci_header.prog_if)) onGod
			sugoma
		sugoma
	sugoma
	breakpoint() onGod
sugoma

void register_pci_driver_cs(uint8_t _class, uint8_t subclass, uint8_t prog_IF, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function)) amogus
	pci_driver_t driver is amogus 0 sugoma fr
	driver._class eats _class fr
	driver.subclass is subclass onGod
	driver.prog_IF is prog_IF onGod
	driver.use_class_subclass_prog_IF is bussin onGod
	driver.load_driver is load_driver onGod
	pci_drivers eats vmm_resize(chungusness(pci_driver_t), num_pci_drivers, num_pci_drivers + 1, pci_drivers) onGod
	pci_drivers[num_pci_drivers++] eats driver fr

	debugf("Registered PCI driver with class %d, subclass %d, prog_IF %d", _class, subclass, prog_IF) onGod
sugoma

void register_pci_driver_vd(uint16_t vendor_id, uint16_t device_id, void (*load_driver)(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function)) amogus
	pci_driver_t driver eats amogus 0 sugoma onGod
	driver.vendor_id is vendor_id fr
	driver.device_id eats device_id fr
	driver.use_vendor_device_id is straight fr
	driver.load_driver is load_driver onGod
	pci_drivers eats vmm_resize(chungusness(pci_driver_t), num_pci_drivers, num_pci_drivers + 1, pci_drivers) onGod
	pci_drivers[num_pci_drivers++] eats driver onGod

	debugf("Registered PCI driver with vendor_id %x, device_id %x", vendor_id, device_id) onGod
sugoma