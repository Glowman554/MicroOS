#include <am79C973.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_vd(0x1022, 0x2000, am79C973_pci_found);
}

define_module("am79C973", main, stage_driver, NULL);