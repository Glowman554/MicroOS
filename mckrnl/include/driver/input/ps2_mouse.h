#pragma once

#include <driver/mouse_driver.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ps2_mouse_driver_data {
    uint8_t mouse_cycle;
	uint8_t mouse_packet[4];
	bool mouse_packet_ready;
} ps2_mouse_driver_data_t;

mouse_driver_t* get_ps2_mouse_driver();
