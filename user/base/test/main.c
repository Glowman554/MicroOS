#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <non-standard/sys/env.h>
#include <non-standard/sys/spawn.h>
#include <non-standard/buildin/path.h>

typedef struct {
	char** argv;
	bool (*test)(pipe_t* stdout_pipe, int exit_code);
} test_t;

#define PIPE_BUFFER_SIZE 65536
pipe_t global_stdout_pipe;

void set_wait_and_yield_term() {
    set_env(SYS_ENV_TASK_SET_WAIT_TIME, (void*)100);
    yield();
}

int spawn_and_capture(char** argv, pipe_t* stdout_pipe) {
    char* executable = search_executable(argv[0]);
    if (!executable) {
        return -1;
    }

    stdout_pipe->pos = 0;
    memset(stdout_pipe->buffer, 0, PIPE_BUFFER_SIZE);

    const char** envp = (const char**)env(SYS_GET_ENVP_ID);

    set_env(SYS_ENV_PIN, (void*)1);
    int pid = spawn(executable, (const char**)argv, envp);

    if (pid == -1) {
        set_env(SYS_ENV_PIN, (void*)0);
        free(executable);
        return -1;
    }

    set_pipe(pid, stdout_pipe, PIPE_STDOUT);
    set_env(SYS_ENV_PIN, (void*)0);

    while (get_proc_info(pid)) {
        set_wait_and_yield_term();
    }

    free(executable);
    return get_exit_code(pid);
}




bool test_exit_code(pipe_t* stdout_pipe, int exit_code) {
	return exit_code == 0;
}

bool test_calc(pipe_t* stdout_pipe, int exit_code) {
	if (exit_code != 0) {
		return false;
	}
	return strcmp(stdout_pipe->buffer, "7\n") == 0;
}

bool test_hello(pipe_t* stdout_pipe, int exit_code) {
	if (exit_code != 0) {
		return false;
	}
	return strcmp(stdout_pipe->buffer, "Hello world!\n") == 0;
}

bool test_hello_libc(pipe_t* stdout_pipe, int exit_code) {
	if (exit_code != 0) {
		return false;
	}
	return strcmp(stdout_pipe->buffer, "Hello, world 69!\n") == 0;
}

bool test_wc(pipe_t* stdout_pipe, int exit_code) {
	if (exit_code != 0) {
		return false;
	}
	return strcmp(stdout_pipe->buffer, "34 76 420 initrd:/examples/hello.asm\n") == 0;
}


test_t tests[] = {
{
		.argv = (char*[]){ "calc", "1", "+", "2", "*", "3", NULL },
		.test = test_calc
	},
{
		.argv = (char*[]){ "fasm", "initrd:/examples/hello.asm", "tmp:/hello.elf", NULL },
		.test = test_exit_code
	},
{
		.argv = (char*[]){ "tmp:/hello.elf", NULL },
		.test = test_hello
	},
{
		.argv = (char*[]){ "fasm", "initrd:/examples/hello-libc.asm", "tmp:/hello-libc.o", NULL },
		.test = test_exit_code
	},
{
		.argv = (char*[]){ "load", "tmp:/hello-libc.o", NULL },
		.test = test_hello_libc
	},
{
		.argv = (char*[]){ "fasm", "initrd:/examples/wc.asm", "tmp:/wc.o", NULL },
		.test = test_exit_code
	},
{
		.argv = (char*[]){ "load", "tmp:/wc.o", "initrd:/examples/hello.asm", NULL },
		.test = test_wc
	},
{
		.argv = (char*[]){ "wc", "initrd:/examples/hello.asm", NULL },
		.test = test_wc
	},
{
		.argv = (char*[]){ "mcc", "initrd:/examples/hello.c", "-o", "tmp:/hello-c.elf", NULL },
		.test = test_exit_code
	},
{
		.argv = (char*[]){ "tmp:/hello-c.elf", NULL },
		.test = test_hello
	},
{
		.argv = (char*[]){ "mcc", "initrd:/examples/hello-libc.c", "-r", "-o", "tmp:/hello-libc-c.o", NULL },
		.test = test_exit_code
	},
{
		.argv = (char*[]){ "load", "tmp:/hello-libc-c.o", NULL },
		.test = test_hello
	},
};

int main(int argc, char* argv[], char* envp[]) {
    global_stdout_pipe.buffer = malloc(PIPE_BUFFER_SIZE);
    global_stdout_pipe.size = PIPE_BUFFER_SIZE;
    global_stdout_pipe.pos = 0;
    memset(global_stdout_pipe.buffer, 0, PIPE_BUFFER_SIZE);

	int success = 0;
	int failed = 0;

	for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
		char** argv = tests[i].argv;
		printf("test[%d]: ", i);
		for (int j = 0; argv[j]; j++) {
			printf("%s ", argv[j]);
		}
		printf("\n");

		int exit_code = spawn_and_capture(argv, &global_stdout_pipe);

		if (tests[i].test(&global_stdout_pipe, exit_code)) {
			success++;
		} else {
			failed++;
			printf("test[%d]: Failed %s\n", i, argv[0]);

			printf("test[%d]: ", i);
			for (int j = 0; j < global_stdout_pipe.size; j++) {
				if (global_stdout_pipe.buffer[j] == '\n') {
					printf("\ntest[%d]: ", i);
				} else {
					putchar(global_stdout_pipe.buffer[j]);
				}
			}
		}
	}

	printf("test: success: %d, failed: %d\n", success, failed);

	return 0;
}