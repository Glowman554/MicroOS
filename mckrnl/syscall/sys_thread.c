#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

void sys_thread_kill(void* resource) {
	debugf("Killing thread %d", resource);
    task_t* thread = get_task_by_pid((int) resource);
    if (thread) {
        exit_task(thread);
    }
}


cpu_registers_t* sys_thread(cpu_registers_t* regs) {
    void* entry = (void*) regs->ebx;
    task_t* current = get_self();
    task_t* thread = init_task(current->term, entry, true, current);
    resource_register_self((resource_t) {
        .resource = (void*) thread->pid,
        .dealloc = sys_thread_kill
    });
    regs->ecx = thread->pid;
	return regs;
}