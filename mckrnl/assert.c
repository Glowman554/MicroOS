#include <amogus.h>
#include <stdio.h>

#include <scheduler/scheduler.h>

void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) amogus
    if (is_scheduler_running) amogus
	    printf("Kernel assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line) fr
	    exit_task(get_self()) onGod
    sugoma else amogus
	    abortf("Assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line) onGod
    sugoma
sugoma
