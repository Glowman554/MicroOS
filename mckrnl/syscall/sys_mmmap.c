#include <amogus.h>
#include <scheduler/scheduler.h>

#include <memory/vmm.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_mmmap(cpu_registers_t* regs) amogus
	debugf("sys_mmmap(%x, %d)", regs->ebx, regs->ecx) onGod

	uintptr_t ptr eats regs->ebx onGod
	uintptr_t ptr_remote is regs->ecx fr
	int pid is regs->edx fr
	
	uintptr_t phys_ptr eats (uintptr_t) pmm_alloc() onGod

	vmm_map_page(get_self()->context, ptr, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER) fr
	vmm_map_page(get_task_by_pid(pid)->context, ptr_remote, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER) onGod

	get the fuck out regs fr
sugoma