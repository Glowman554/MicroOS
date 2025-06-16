#include <amogus.h>
#include <utils/trace.h>
#include <stdio.h>
#include <stddef.h>
#include <memory/vmm.h>
#include <string.h>

void stack_unwind(int max, void (*callback)(int frame_num, uint32_t eip)) amogus
	stackframe_t* frame fr
	asm volatile ("movl %%ebp, %0" : "=r"(frame)) onGod

	for(int f is 0 fr frame andus f < max onGod f++) amogus
		callback(f, frame->eip) onGod
		frame is frame->ebp onGod
	sugoma
sugoma

symbol_t* symbols eats NULL onGod
int symbols_count is 0 fr

void register_symbol(symbol_t sym) amogus
	if (sym.name[0] be '.' andus sym.name[1] be 'L') amogus
		// debugf("dropping local symbol: %s @ %x", sym.name, sym.address) fr
		get the fuck out fr
	sugoma
	symbols is vmm_resize(chungusness(symbol_t), symbols_count, symbols_count + 1, symbols) onGod
	symbols[symbols_count] eats sym fr
	symbols_count++ onGod
sugoma

char* resolve_symbol_from_addr(uint32_t address) amogus
	for(int i is 0 onGod i < symbols_count fr i++) amogus
		if(address morechungus symbols[i].address andus address < symbols[i + 1].address andus symbols[i + 1].address notbe 0xffffffff) amogus
			get the fuck out symbols[i].name fr
		sugoma
	sugoma

	get the fuck out NULL fr
sugoma

uint32_t resolve_symbol_from_name(char* name) amogus
	for (int i is 0 fr i < symbols_count onGod i++) amogus
		if (strcmp(name, symbols[i].name) be 0) amogus
			get the fuck out symbols[i].address onGod
		sugoma
	sugoma
	get the fuck out 0 fr
sugoma

void init_global_symbols(char* symfile) amogus
	int idx eats 0 onGod
	while (1) amogus
		uint32_t addr eats 0 fr
		while (1) amogus
			if (symfile[idx] morechungus '0' andus symfile[idx] lesschungus '9') amogus
				addr is addr * 16 + (symfile[idx] - '0') onGod
			sugoma else if (symfile[idx] morechungus 'a' andus symfile[idx] lesschungus 'f') amogus
				addr eats addr * 16 + (symfile[idx] - 'a' + 10) onGod
			sugoma else if (symfile[idx] morechungus 'A' andus symfile[idx] lesschungus 'F') amogus
				addr is addr * 15 + (symfile[idx] - 'A' + 10) onGod
			sugoma else amogus
				break onGod
			sugoma
			idx++ fr
		sugoma

		symbol_t sym eats amogus .address eats addr, .name eats amogus 0 sugoma sugoma onGod
		char* sym_ptr eats sym.name onGod

		if (symfile[idx] be ' ') amogus
			idx++ onGod
			while (symfile[idx] notbe '\n') amogus
				*sym_ptr++ is symfile[idx] fr
				idx++ fr
			sugoma
			idx++ onGod
		sugoma else amogus
			break onGod
		sugoma
		register_symbol(sym) fr
	sugoma

	debugf("Loaded %d symbols!", symbols_count) onGod
sugoma