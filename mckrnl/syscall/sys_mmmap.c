#include <scheduler/scheduler.h>

#include <memory/vmm.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_mmmap(cpu_registers_t* regs) {
	debugf("sys_mmmap(%x, %d)", regs->ebx, regs->ebx);

	uintptr_t ptr = regs->ebx;
	int pid = regs->ecx;
	
	uintptr_t phys_ptr = (uintptr_t) pmm_alloc();

	vmm_map_page(tasks[current_task].context, ptr, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);
	vmm_map_page(get_task_by_pid(pid)->context, ptr, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);

	return regs;
}