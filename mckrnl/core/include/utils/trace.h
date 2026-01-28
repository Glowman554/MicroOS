#pragma once

#include <stdint.h>

typedef struct stackframe {
	struct stackframe* ebp;
	uint32_t eip;
} stackframe_t;

void stack_unwind(int max, void (*callback)(int frame_num, uint32_t eip));
void stack_unwind_frame(stackframe_t* frame, int max, void (*callback)(int frame_num, uint32_t eip));

typedef struct symbol {
	uint32_t address;
	char name[128];
} symbol_t;

extern symbol_t* symbols;
extern int symbols_count;

void init_global_symbols(char* symfile);
char* resolve_symbol_from_addr(uint32_t address);
uint32_t resolve_symbol_from_name(char* name);