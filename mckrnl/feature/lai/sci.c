#include <lai/helpers/sci.h>

#include <driver/acpi/rsdp.h>
#include <driver/power_driver.h>

#include <interrupts/interrupts.h>

#include <stdio.h>

int get_sci_interrupt() {
	fadt_table_t* fadt = (fadt_table_t*) find_SDT("FACP", 0);

	return fadt->sci_interrupt + 0x20;
}

cpu_registers_t* sci_interrupt_handler(cpu_registers_t* registers, void* context) {
	uint16_t ev = lai_get_sci_event();

	if (ev & ACPI_POWER_BUTTON) {
		debugf(SPAM, "sci: power button");
		global_power_driver->shutdown(global_power_driver);
	} else if (ev & ACPI_SLEEP_BUTTON) {
		debugf(SPAM, "sci: sleep button");
	} else if (ev & ACPI_WAKE) {
		debugf(SPAM, "sci: sleep wake up");
	} else {
		debugf(SPAM, "sci: unknown");
	}	

	return registers;
}

void register_sci_interrupt() {
	int sci_interrupt = get_sci_interrupt();
	debugf(SPAM, "Registering SCI interrupt at %d", sci_interrupt);

	register_interrupt_handler(sci_interrupt, sci_interrupt_handler, NULL);
}