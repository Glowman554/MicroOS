#include <power.h>

#include <lai/helpers/pm.h>

#include <stdio.h>

bool lai_power_driver_is_device_present(driver_t* driver) {
	return true;
}

char* lai_power_driver_get_device_name(driver_t* driver) {
	return "lai_power";
}

void lai_power_driver_init(driver_t* driver) {
    global_power_driver = (power_driver_t*) driver;
}

void lai_power_driver_reboot(power_driver_t* driver) {
    lai_api_error_t err = lai_acpi_reset();
    abortf(false, "lai_acpi_reset() = %d", err);
}

void lai_power_driver_shutdown(power_driver_t* driver) {
    lai_api_error_t err = lai_enter_sleep(5);
    abortf(false, "lai_enter_sleep() = %d", err);
}


power_driver_t lai_power_driver = {
	.driver = {
		.is_device_present = lai_power_driver_is_device_present,
		.init = lai_power_driver_init,
		.get_device_name = lai_power_driver_get_device_name,
	},
	.shutdown = lai_power_driver_shutdown,
	.reboot = lai_power_driver_reboot,
};
