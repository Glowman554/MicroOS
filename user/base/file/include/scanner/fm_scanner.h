#pragma once

#include <stdint.h>

struct note_t {
	uint16_t length_ms;
	uint16_t note;
};

struct foxm_t {
	uint32_t magic; // 0xf0f0baba
	struct note_t notes[];
};

#define FM_MAGIC 0xf0f0baba