#include <scheduler/scheduler.h>

#include <memory/vmm.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <stddef.h>

void sys_mmmap_dealloc(void* resource) {
	debugf("Freeing %x", resource);
	pmm_free(resource);
}


cpu_registers_t* sys_mmmap(cpu_registers_t* regs) {
	debugf("sys_mmmap(%x, %d)", regs->ebx, regs->ecx);

	uintptr_t ptr = regs->ebx;
	uintptr_t ptr_remote = regs->ecx;
	int pid = regs->edx;

	void* old_phys_ptr = vmm_lookup(ptr, get_self()->context);
	if (old_phys_ptr != NULL) {
		// We might be leaking the page
		// (this might happen since it is not guaranteed that its mapped in both processes)
		debugf("WARNING: Possible leaked page %x", old_phys_ptr);
	}

	void* phys_ptr = pmm_alloc();
	vmm_map_page(get_self()->context, ptr, (uintptr_t) phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);

	task_t* other = get_task_by_pid(pid);
	if (other) {
		vmm_map_page(other->context, ptr_remote, (uintptr_t) phys_ptr, PTE_PRESENT | PTE_WRITE | PTE_USER);
	} else {
		debugf("Failed to map tunnel for %d", pid);
	}

	return regs;
}