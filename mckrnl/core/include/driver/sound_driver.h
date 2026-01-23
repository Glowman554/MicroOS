#pragma once

#include <stdint.h>
#include <driver/driver.h>

typedef struct sound_driver {
	driver_t driver;
	uint64_t (*write_pcm)(struct sound_driver*, uint8_t* buffer, uint64_t size);
	uint32_t (*get_sample_rate)(struct sound_driver*);
} sound_driver_t;

extern sound_driver_t* global_sound_driver;