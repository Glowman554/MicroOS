#include <scheduler/pipe.h>
#include <stdio.h>

bool has_pipe(task_t* task, int pipeid) {
    return task->pipe[pipeid] != 0;
}

int read_pipe(task_t* task, int pipeid, char* buffer, uint64_t size) {
    task_t* other = get_task_by_pid(task->pipe_source[pipeid]);
	if (other) {
		pipe_t pipe;
		vmm_read_context(task->pipe[pipeid], &pipe, sizeof(pipe_t), other->context);


        int can_read = size;
        if (pipe.size - pipe.pos < can_read) {
            can_read = pipe.size - pipe.pos;
            debugf("read_pipe: pipe empty, read only %d bytes", can_read);
        }

        vmm_read_context(&pipe.buffer[pipe.pos], buffer, can_read, other->context);
				
		pipe.pos += can_read;
		vmm_write_context(task->pipe[pipeid], &pipe, sizeof(pipe_t), other->context);

        return can_read;
    }

    return 0;
}

int write_pipe(task_t* task, int pipeid, char* buffer, uint64_t size) {
    task_t* other = get_task_by_pid(task->pipe_source[pipeid]);
	if (other) {
		pipe_t pipe;
		vmm_read_context(task->pipe[pipeid], &pipe, sizeof(pipe_t), other->context);

		int to_write = size;
		if (pipe.size - pipe.pos < to_write) {
			to_write = pipe.size - pipe.pos;
			debugf("write_pipe: pipe full, wrote only %d bytes", to_write);
		}
        
        vmm_write_context(&pipe.buffer[pipe.pos], buffer, to_write, other->context);
        
        pipe.pos += to_write;
        vmm_write_context(task->pipe[pipeid], &pipe, sizeof(pipe_t), other->context);

        return to_write;
	}

    return 0;
}
