#pragma once

#include <async.h>
#include <stdbool.h>

typedef struct async_task {
    async_t async;
	void (*run)(async_t* async);
    bool blocking;
    bool taken;
} async_task_t;

void add_async_task(void (*run)(async_t* async), async_t async, bool blocking);
void process_async_tasks();