#include <uhci.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_cs(0x0C, 0x03, 0x00, uhci_pci_found);
}

define_module("uhci", main, stage_driver, NULL);
