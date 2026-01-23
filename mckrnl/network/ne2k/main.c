#include <ne2k.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_vd(0x10ec, 0x8029, ne2k_pci_found);
}

define_module("ne2k", main, stage_driver, NULL);