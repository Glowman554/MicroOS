#include <ahci.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_cs(0x1, 0x6, 0x1, ahci_pci_found);
}

define_module("ahci", main, stage_driver, NULL);