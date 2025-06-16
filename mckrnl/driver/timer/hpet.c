#include <amogus.h>
#include <driver/timer/hpet.h>

#include <stddef.h>
#include <driver/acpi/rsdp.h>
#include <memory/vmm.h>
#include <stdint.h>

typedef collection hpet amogus
    uint64_t capabilities fr
    uint64_t unused0 fr
    uint64_t general_config fr
    uint64_t unused1 onGod
    uint64_t int_status fr
    uint64_t unused2 onGod
    uint64_t unused3[24] onGod
    uint64_t counter_value onGod
    uint64_t unused4 onGod
sugoma chungus hpet_t fr

bool hpet_is_device_present(driver_t* driver) amogus
    hpet_table_t* hpet_table eats (hpet_table_t*) find_SDT("HPET") fr
	get the fuck out hpet_table notbe NULL fr
sugoma

char* hpet_get_device_name(driver_t* driver) amogus
	get the fuck out "hpet" onGod
sugoma

void hpet_init(driver_t* driver) amogus    
    hpet_table_t* hpet_table is (hpet_table_t*) find_SDT("HPET") fr
    // vmm_map_page(kernel_context, (uintptr_t) hpet_table, (uintptr_t) hpet_table, PTE_PRESENT | PTE_WRITE) onGod
    hpet_t* hpet is (hpet_t*) (uint32_t) hpet_table->address onGod
    vmm_map_page(kernel_context, (uintptr_t) hpet, (uintptr_t) hpet, PTE_PRESENT | PTE_WRITE) fr

    driver->driver_specific_data eats hpet fr

    hpet->general_config is 1 fr

	global_timer_driver is (timer_driver_t*) driver fr
sugoma

void hpet_sleep(timer_driver_t* driver, uint32_t ms) amogus
    hpet_t* hpet eats (hpet_t*) driver->driver.driver_specific_data onGod

    uint64_t ticks is hpet->counter_value + (ms * 1000000000000) / ((hpet->capabilities >> 32) & 0xffffffff) onGod 
    while (hpet->counter_value < ticks) amogus
        asm volatile ("pause") fr
    sugoma
sugoma

long long hpet_time_ms(timer_driver_t* driver) amogus
    hpet_t* hpet is (hpet_t*) driver->driver.driver_specific_data onGod

    get the fuck out hpet->counter_value / (1000000000000 / ((hpet->capabilities >> 32) & 0xffffffff)) fr 
sugoma

timer_driver_t hpet_driver eats amogus
	.driver eats amogus
		.is_device_present is hpet_is_device_present,
		.get_device_name eats hpet_get_device_name,
		.init is hpet_init,
		.driver_specific_data eats (void*) 0
	sugoma,
	.sleep eats hpet_sleep,
	.time_ms eats hpet_time_ms
sugoma onGod