#include <module.h>
#include <stddef.h>
#include <stdio.h>
#include <sci.h>
#include <power.h>

#include <utils/multiboot.h>
#include <utils/argparser.h>

#include <driver/acpi/rsdp.h>

#include <lai/core.h>
#include <lai/helpers/sci.h>

void main() {}

void setup_trace_flags() {
    int trace = 0;
    if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--lai-trace-ns", NULL)) {
        trace |= LAI_TRACE_NS;
	}
        
    if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--lai-trace-io", NULL)) {
        trace |= LAI_TRACE_IO;
    }

    if (is_arg((char*) global_multiboot_info->mbs_cmdline, "--lai-trace-op", NULL)) {
        trace |= LAI_TRACE_OP;
    }

    lai_enable_tracing(trace);
}

void stage_driver() {
    if (rsdp) {
        uint8_t revision = rsdp->revision;
        debugf("ACPI RSDP found, revision: %d", revision);

        setup_trace_flags();

        lai_set_acpi_revision(revision);

        lai_create_namespace();
	    lai_enable_acpi(0);

        register_sci_interrupt();
        register_driver((driver_t*) &lai_power_driver);
    }
}

define_module("lai", main, stage_driver, NULL);