#include <scheduler/loader.h>
#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <stdio.h>
#include <string.h>


const char* shebangs[] = {
	"#!",
	"//!",
	"--!",
    ";!"
};

int load_executable(int term, char* path, char** argv, char** envp) {
    file_t* file = vfs_open(path, FILE_OPEN_MODE_READ);
	if (!file) {
		debugf("Failed to open %s", path);
		return -1;
	}

	task_t* current = get_self();

	void* buffer = vmm_alloc(file->size / 4096 + 1);
	vfs_read(file, buffer, file->size, 0);

    bool is_shebang = false;
    int shebang_offset = 0;
    for (int i = 0; i < sizeof(shebangs) / sizeof(shebangs[0]); i++) {
        int len = strlen(shebangs[i]);
        if (file->size >= len) {
            if (memcmp(buffer, shebangs[i], len) == 0) {
                is_shebang = true;
                shebang_offset = len;
                break;
            }
        }
    }
	
    int pid = -1;
    if (is_shebang) {
        char executor[256] = { 0 };
        int j = 0;
        for (j = 0; j < sizeof(executor); j++) {
            char c = ((char*) buffer)[shebang_offset + j];
            if (c == '\n' || c == '\0') {
                break;
            }
            executor[j] = c;
        }
        executor[j] = '\0';

        debugf("shebang detected, executor: %s", executor);

        int argc = 0;
        while (argv[argc] != NULL) {
            argc++;
        }
        argc--; // exclude original argv[0]

        char* new_argv[argc + 3];
        new_argv[0] = executor;
        new_argv[1] = path;
        for (int i = 0; i < argc; i++) {
            new_argv[i + 2] = argv[i + 1];
        }
        new_argv[argc + 2] = NULL;

        pid = load_executable(current->term, executor, new_argv, envp);
    } else {
        pid = init_executable(current->term, buffer, argv, envp);
    }
	
    vmm_free(buffer, file->size / 4096 + 1);
	vfs_close(file);

	return pid;
}