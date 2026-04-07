#pragma once

#include <stdint.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
} psf1_header_t;