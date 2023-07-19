#pragma once

#include <stdint.h>

typedef struct stackframe {
	struct stackframe* ebp;
	uint32_t eip;
} stackframe_t;

void stack_unwind(int max, void (*callback)(int frame_num, uint32_t eip));