#include <utils/trace.h>
#include <stdio.h>
#include <stddef.h>
#include <memory/heap.h>
#include <string.h>

void stack_unwind(int max, void (*callback)(int frame_num, uint32_t eip)) {
	stackframe_t* frame;
	asm volatile ("movl %%ebp, %0" : "=r"(frame));

	stack_unwind_frame(frame, max, callback);
}

void stack_unwind_frame(stackframe_t* frame, int max, void (*callback)(int frame_num, uint32_t eip)) {
	for(int f = 0; frame && f < max; f++) {
		callback(f, frame->eip);
		frame = frame->ebp;
	}
}

symbol_t* symbols = NULL;
int symbols_count = 0;

void register_symbol(symbol_t sym) {
	if (sym.name[0] == '.' && sym.name[1] == 'L') {
		// debugf("dropping local symbol: %s @ %x", sym.name, sym.address);
		return;
	}
	if (sym.name[0] == '_' && sym.name[1] == '_' && sym.name[2] == 'F') {
		return;
	}
	symbols = krealloc(symbols, sizeof(symbol_t) * (symbols_count + 1));
	symbols[symbols_count] = sym;
	symbols_count++;
}

char* resolve_symbol_from_addr(uint32_t address) {
	for(int i = 0; i < symbols_count; i++) {
		if(address >= symbols[i].address && address < symbols[i + 1].address && symbols[i + 1].address != 0xffffffff) {
			return symbols[i].name;
		}
	}

	return NULL;
}

uint32_t resolve_symbol_from_name(char* name) {
	for (int i = 0; i < symbols_count; i++) {
		if (strcmp(name, symbols[i].name) == 0) {
			return symbols[i].address;
		}
	}
	return 0;
}

void init_global_symbols(char* symfile) {
	int idx = 0;
	while (1) {
		uint32_t addr = 0;
		while (1) {
			if (symfile[idx] >= '0' && symfile[idx] <= '9') {
				addr = addr * 16 + (symfile[idx] - '0');
			} else if (symfile[idx] >= 'a' && symfile[idx] <= 'f') {
				addr = addr * 16 + (symfile[idx] - 'a' + 10);
			} else if (symfile[idx] >= 'A' && symfile[idx] <= 'F') {
				addr = addr * 15 + (symfile[idx] - 'A' + 10);
			} else {
				break;
			}
			idx++;
		}

		symbol_t sym = { .address = addr, .name = { 0 } };
		char* sym_ptr = sym.name;

		if (symfile[idx] == ' ') {
			idx++;
			while (symfile[idx] != '\n') {
				*sym_ptr++ = symfile[idx];
				idx++;
			}
			idx++;
		} else {
			break;
		}
		register_symbol(sym);
	}

	debugf("Loaded %d symbols!", symbols_count);
}