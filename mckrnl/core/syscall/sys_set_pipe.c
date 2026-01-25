#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <config.h>
#include <stdio.h>

cpu_registers_t* sys_set_pipe(cpu_registers_t* regs) {
    int pid = regs->ebx;
    pipe_t* pipe = (pipe_t*) regs->ecx;
    int output = regs->edx;

    task_t* self = get_self();
    task_t* other = get_task_by_pid(pid);
    if (!other) {
        abortf(true, "sys_set_pipe: task with pid %d not found", pid);
    }

    if (output < 0 || output >= 3) {
        abortf(true, "sys_set_pipe: invalid pipe index %d", output);
    }

    debugf("sys_set_pipe: setting pipe for pid %d, output %d", pid, output);
    debugf("sys_set_pipe: buffer=%p, size=%d, pos=%d", pipe->buffer, pipe->size, pipe->pos);

    other->pipe[output] = pipe;
    other->pipe_source[output] = self->pid;

	return regs;
}