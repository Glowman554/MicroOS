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
	if (task->stdin_pipe_size + size >= 4096) {
		// For simplicity, just use first 4096 bytes
		size = 4096 - task->stdin_pipe_size - 1;
		if (size == 0) {
			return regs;
		}
	}

	// Copy data to stdin pipe
	memcpy(task->stdin_pipe + task->stdin_pipe_size, buffer, size);
	task->stdin_pipe_size += size;
	task->stdin_pipe[task->stdin_pipe_size] = '\0';

	return regs;
}
