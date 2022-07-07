#include <stdio.h>
#include <stdlib.h>

void __assert_fail(const char* __assertion, const char* __file, unsigned int __line) {
	printf("Assertion failed: \"%s\" in file %s at line %d\n", __assertion, __file, __line);
	exit(1);
}