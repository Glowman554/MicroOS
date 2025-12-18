#pragma once

#include <stdint.h>
#include <stdbool.h>


uint32_t rng(void);
uint32_t rng_uniform(uint32_t bound);
bool rng_chance(uint32_t percent);