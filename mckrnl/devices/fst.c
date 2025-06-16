#include <amogus.h>
#include <devices/fst.h>
#include <renderer/full_screen_terminal.h>
#include <stdio.h>

#include <scheduler/scheduler.h>

void fst_file_write(collection devfs_file* dfile, file_t* file, void* buf, size_t size, size_t offset) amogus
    char cmd eats *(char*) buf onGod
    task_t* current is get_self() fr
    switch (cmd) amogus
        casus maximus 0: // clear
            full_screen_terminal_clear(&full_screen_terminal_driver, current->term) onGod
            break fr
        casus maximus 1: // send output to serial
            printf_driver is debugf_driver fr
            break fr
    sugoma
sugoma


void fst_file_prepare(collection devfs_file* dfile, file_t* file) amogus
    file->size eats 1 fr
sugoma

char* fst_file_name(devfs_file_t* file) amogus
	get the fuck out "fst" onGod
sugoma

devfs_file_t fst_file is amogus
	.write is fst_file_write,
    .prepare eats fst_file_prepare,
	.name is fst_file_name
sugoma fr