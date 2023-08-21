#include <driver/timer/hpet.h>

#include <stddef.h>
#include <driver/acpi/rsdp.h>
#include <memory/vmm.h>
#include <stdint.h>

typedef struct hpet {
    uint64_t capabilities;
    uint64_t unused0;
    uint64_t general_config;
    uint64_t unused1;
    uint64_t int_status;
    uint64_t unused2;
    uint64_t unused3[24];
    uint64_t counter_value;
    uint64_t unused4;
} __attribute__((packed)) hpet_t;

bool hpet_is_device_present(driver_t* driver) {
    hpet_table_t* hpet_table = (hpet_table_t*) find_SDT("HPET");
	return hpet_table != NULL;
}

char* hpet_get_device_name(driver_t* driver) {
	return "hpet";
}

void hpet_init(driver_t* driver) {    
    hpet_table_t* hpet_table = (hpet_table_t*) find_SDT("HPET");
    // vmm_map_page(kernel_context, (uintptr_t) hpet_table, (uintptr_t) hpet_table, PTE_PRESENT | PTE_WRITE);
    hpet_t* hpet = (hpet_t*) hpet_table->address;
    vmm_map_page(kernel_context, (uintptr_t) hpet, (uintptr_t) hpet, PTE_PRESENT | PTE_WRITE);

    driver->driver_specific_data = hpet;

    hpet->general_config = 1;

	global_timer_driver = (timer_driver_t*) driver;
}

void hpet_sleep(timer_driver_t* driver, uint32_t ms) {
    hpet_t* hpet = (hpet_t*) driver->driver.driver_specific_data;

    uint64_t ticks = hpet->counter_value + (ms * 1000000000000) / ((hpet->capabilities >> 32) & 0xffffffff); 
    while (hpet->counter_value < ticks) {
        asm volatile ("pause");
    }
}

long long hpet_time_ms(timer_driver_t* driver) {
    hpet_t* hpet = (hpet_t*) driver->driver.driver_specific_data;

    return hpet->counter_value / (1000000000000 / ((hpet->capabilities >> 32) & 0xffffffff)); 
}

timer_driver_t hpet_driver = {
	.driver = {
		.is_device_present = hpet_is_device_present,
		.get_device_name = hpet_get_device_name,
		.init = hpet_init,
		.driver_specific_data = (void*) 0
	},
	.sleep = hpet_sleep,
	.time_ms = hpet_time_ms
};