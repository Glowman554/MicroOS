#include <ac97.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
    register_pci_driver_cs(0x04, 0x01, 0x00, ac97_pci_found);
	// register_pci_driver_vd(0x8086, 0x3A3E, ac97_pci_found);
	// register_pci_driver_vd(0x8086, 0x3A6E, ac97_pci_found);
	// register_pci_driver_vd(0x1022, 0x15E3, ac97_pci_found);
}

define_module("ac97", main, stage_driver, NULL);