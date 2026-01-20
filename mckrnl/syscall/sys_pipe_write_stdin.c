#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>
#include <memory/heap.h>

cpu_registers_t* sys_pipe_write_stdin(cpu_registers_t* regs) {
	int pid = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t size = regs->edx;

	if (buffer == NULL) {
		abortf(true, "sys_pipe_write_stdin: buffer is NULL");
		return regs;
	}

	task_t* task = get_task_by_pid(pid);
	if (task == NULL) {
		abortf(true, "sys_pipe_write_stdin: invalid pid %d", pid);
		return regs;
	}

	if (task->stdin_pipe == NULL) {
		// No pipe configured
		return regs;
	}

	// Resize pipe buffer if needed
	size_t available_space = 4096 - task->stdin_pipe_size;
	if (available_space <= 1) {
		// Buffer is full, cannot write more
		return regs;
	}

	// Limit write size to available space (minus 1 for null terminator)
	size_t write_size = size;
	if (write_size > available_space - 1) {
		write_size = available_space - 1;
	}

	if (write_size == 0) {
		return regs;
	}

	// Copy data to stdin pipe
	memcpy(task->stdin_pipe + task->stdin_pipe_size, buffer, write_size);
	task->stdin_pipe_size += write_size;
	task->stdin_pipe[task->stdin_pipe_size] = '\0';

	return regs;
}
