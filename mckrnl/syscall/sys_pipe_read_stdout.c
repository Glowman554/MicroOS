#include <syscall/syscalls.h>

#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>
#include <memory/vmm.h>

cpu_registers_t* sys_pipe_read_stdout(cpu_registers_t* regs) {
	int pid = regs->ebx;
	void* buffer = (void*) regs->ecx;
	size_t max_size = regs->edx;

	if (buffer == NULL) {
		abortf(true, "sys_pipe_read_stdout: buffer is NULL");
		return regs;
	}

	task_t* task = get_task_by_pid(pid);
	if (task == NULL) {
		abortf(true, "sys_pipe_read_stdout: invalid pid %d", pid);
		return regs;
	}

	if (task->stdout_pipe == NULL) {
		// No pipe, return 0 bytes read
		regs->esi = 0;
		return regs;
	}

	// Copy pipe contents to buffer
	size_t to_copy = (task->stdout_pipe_size < max_size) ? task->stdout_pipe_size : max_size;
	memcpy(buffer, task->stdout_pipe, to_copy);
	
	// Free the pipe buffer now that it's been read
	size_t pages = TO_PAGES(task->stdout_pipe_capacity);
	vmm_free(task->stdout_pipe, pages);
	task->stdout_pipe = NULL;
	task->stdout_pipe_size = 0;
	task->stdout_pipe_capacity = 0;
	
	// Return number of bytes copied
	regs->esi = to_copy;

	return regs;
}
