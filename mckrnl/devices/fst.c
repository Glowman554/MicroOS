#include <devices/fst.h>
#include <renderer/full_screen_terminal.h>
#include <stdio.h>

#include <scheduler/scheduler.h>

void fst_file_write(struct devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) {
    char cmd = *(char*) buf;
    task_t* current = get_self();
    switch (cmd) {
        case 0: // clear
            full_screen_terminal_clear(&full_screen_terminal_driver, current->term);
            break;
        case 1: // send output to serial
            printf_driver = debugf_driver;
            break;
    }
}


void fst_file_prepare(struct devfs_file* dfile, file_t* file) {
}

char* fst_file_name(devfs_file_t* file) {
	return "fst";
}

devfs_file_t fst_file = {
	.write = fst_file_write,
    .prepare = fst_file_prepare,
	.name = fst_file_name
};