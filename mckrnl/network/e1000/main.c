#include <e1000.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_vd(0x8086, 0x100E, e1000_pci_found);
}

define_module("e1000", main, stage_driver, NULL);