#pragma once

#include <stdint.h>
#include <stdbool.h>

extern bool ethdump_dump_enabled;

void etherframe_dump(uint8_t* data, uint32_t len);