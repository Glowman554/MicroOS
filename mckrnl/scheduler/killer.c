#include <scheduler/killer.h>

#include <scheduler/scheduler.h>

#include <stdio.h>
#include <interrupts/interrupts.h>
#include <stddef.h>

char* page_fault_get_error(uint32_t error);

cpu_registers_t* division_by_zero_killer(cpu_registers_t* registers, void* _) {
	printf("#DE (Division by zero) \n");

	exit_task(&tasks[current_task]);

	return registers;
}

cpu_registers_t* invalid_opcode_killer(cpu_registers_t* registers, void* _) {
	printf("#UD (Invalid opcode) \n");

	exit_task(&tasks[current_task]);

	return registers;
}

cpu_registers_t* page_fault_killer(cpu_registers_t* registers, void* _) {
	uint32_t cr2;
	asm volatile("mov %%cr2, %0" : "=r" (cr2));

	printf("#PF (Page fault) %s @ 0x%x\n", page_fault_get_error(registers->error), cr2);

	exit_task(&tasks[current_task]);

	return registers;
}

cpu_registers_t* general_protection_fault_killer(cpu_registers_t* registers, void* _) {
	printf("#GP\n");

	exit_task(&tasks[current_task]);

	return registers;
}


void init_killer() {
	debugf("Initialising killers!");

	register_interrupt_handler(0x0, division_by_zero_killer, NULL);
	register_interrupt_handler(0x6, invalid_opcode_killer, NULL);
	register_interrupt_handler(0xd, general_protection_fault_killer, NULL);
	register_interrupt_handler(0xe, page_fault_killer, NULL);
}