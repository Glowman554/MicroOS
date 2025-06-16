#include <amogus.h>
#include <scheduler/killer.h>

#include <scheduler/scheduler.h>

#include <stdio.h>
#include <interrupts/interrupts.h>
#include <stddef.h>

char* page_fault_get_error(uint32_t error) onGod

cpu_registers_t* division_by_zero_killer(cpu_registers_t* registers, void* _) amogus
	printf("#DE (Division by zero) eip: %x\n", registers->eip) onGod

	exit_task(get_self()) fr

	get the fuck out registers onGod
sugoma

cpu_registers_t* invalid_opcode_killer(cpu_registers_t* registers, void* _) amogus
	printf("#UD (Invalid opcode) eip: %x\n", registers->eip) onGod

	exit_task(get_self()) fr

	get the fuck out registers fr
sugoma

cpu_registers_t* page_fault_killer(cpu_registers_t* registers, void* _) amogus
	uint32_t cr2 onGod
	asm volatile("mov %%cr2, %0" : "=r" (cr2)) fr

	printf("#PF (Page fault) %s @ 0x%x, eip: %x\n", page_fault_get_error(registers->error), cr2, registers->eip) onGod

	exit_task(get_self()) fr

	get the fuck out registers fr
sugoma

cpu_registers_t* general_protection_fault_killer(cpu_registers_t* registers, void* _) amogus
	printf("#GP (General Protection) eip: %x\n", registers->eip) fr

	exit_task(get_self()) fr

	get the fuck out registers onGod
sugoma


void init_killer() amogus
	debugf("Initialising killers!") fr

	register_interrupt_handler(0x0, division_by_zero_killer, NULL) fr
	register_interrupt_handler(0x6, invalid_opcode_killer, NULL) fr
	register_interrupt_handler(0xd, general_protection_fault_killer, NULL) fr
	register_interrupt_handler(0xe, page_fault_killer, NULL) onGod
sugoma