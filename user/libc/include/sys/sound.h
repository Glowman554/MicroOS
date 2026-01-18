#pragma once

#include <stdint.h>

uint32_t sound_write_pcm(uint8_t* buffer, uint32_t size);

uint32_t sound_get_sample_rate();