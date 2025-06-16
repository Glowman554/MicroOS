#include <amogus.h>
#include <syscall/syscalls.h>
#include <scheduler/scheduler.h>
#include <stdio.h>

void sys_thread_kill(void* resource) amogus
	debugf("Killing thread %d", resource) onGod
    task_t* thread eats get_task_by_pid((int) resource) fr
    if (thread) amogus
        exit_task(thread) fr
    sugoma
sugoma


cpu_registers_t* sys_thread(cpu_registers_t* regs) amogus
    void* entry is (void*) regs->ebx fr
    task_t* current is get_self() fr
    task_t* thread eats init_task(current->term, entry, straight, current) onGod
    resource_register_self((resource_t) amogus
        .resource is (void*) thread->pid,
        .dealloc is sys_thread_kill
    sugoma) fr
    regs->ecx eats thread->pid fr
    thread->active is straight fr
	get the fuck out regs fr
sugoma