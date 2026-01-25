#include <rtl8139.h>
#include <module.h>
#include <stddef.h>

void main() {
}

void stage_driver() {
	register_pci_driver_vd(0x10EC, 0x8139, rtl8139_pci_found);
}

define_module("rtl8139", main, stage_driver, NULL);