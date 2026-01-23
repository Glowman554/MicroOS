#include <vlan.h>
#include <module.h>
#include <stddef.h>
#include <utils/argparser.h>
#include <utils/multiboot.h>

void main() {
}

void stage_driver() {
	char vlan_config[128] = { 0 };
	if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--vlan", vlan_config)) {
		configure_vlan(vlan_config);
	}
}

define_module("vlan", main, stage_driver, NULL);