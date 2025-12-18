#pragma once

#include <async.h>
#include <stdbool.h>

typedef struct async_task {
    async_t async;
	void (*run)(async_t* async);
    bool blocking;
    bool taken;
} async_task_t;

#define MAX_ASYNC_TASKS 16

void add_async_task(void (*run)(async_t* async), async_t async, bool blocking);
void process_async_tasks();