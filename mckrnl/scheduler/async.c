#include <scheduler/async.h>
#include <config.h>

async_task_t async_tasks[MAX_ASYNC_TASKS] = {0};

void add_async_task(void (*run)(async_t* async), async_t async, bool blocking) {
    for (int i = MAX_ASYNC_TASKS - 1; i >= 0; i--) {
        if (!async_tasks[i].taken) {
            async_tasks[i].run = run;
            async_tasks[i].async = async;
            async_tasks[i].taken = true;
            async_tasks[i].blocking = blocking;
            return;
        }
    }
}

void process_async_tasks() {
    bool blocking_task_run = false;
    for (int i = 0; i < MAX_ASYNC_TASKS; i++) {
        if (async_tasks[i].taken && !is_resolved(&async_tasks[i].async)) {
            if (async_tasks[i].blocking && blocking_task_run) {
                continue;
            }

            async_tasks[i].run(&async_tasks[i].async);
            if (is_resolved(&async_tasks[i].async)) {
                async_tasks[i].taken = false;
            }
            
            if (async_tasks[i].blocking) {
                blocking_task_run = true;
            }
        }
    }
}