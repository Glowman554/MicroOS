#include <stdio.h>

void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) {
	abortf(true, "Kernel assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);
}
