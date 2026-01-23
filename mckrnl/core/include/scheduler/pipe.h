#pragma once

#include <scheduler/scheduler.h>


bool has_pipe(task_t* task, int pipeid);

int read_pipe(task_t* task, int pipeid, char* buffer, uint64_t size);
int write_pipe(task_t* task, int pipeid, char* buffer, uint64_t size);
