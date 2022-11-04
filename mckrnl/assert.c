#include <stdio.h>

#include <scheduler/scheduler.h>

void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) {
    if (is_scheduler_running) {
	    printf("Kernel assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);
	    exit_task(&tasks[current_task]);
    } else {
	    abortf("Assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);
    }
}
