#include <scheduler/scheduler.h>

#include <memory/vmm.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

cpu_registers_t* sys_mmmap(cpu_registers_t* regs) {
	debugf("sys_mmmap(%x, %d)", regs->ebx, regs->ecx);

	uintptr_t ptr = regs->ebx;
	uintptr_t ptr_remote = regs->ecx;
	int pid = regs->edx;
	
	uintptr_t phys_ptr = (uintptr_t) pmm_alloc();

	vmm_map_page(get_self()->context, ptr, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);
	vmm_map_page(get_task_by_pid(pid)->context, ptr_remote, phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);

	return regs;
}