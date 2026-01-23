#include <ata.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_cs(0x1, 0x1, 0x0, ata_pci_found);
}

define_module("ata", main, stage_driver, NULL);