#pragma once

#include <stdbool.h>

#ifndef __LIBC_USE_NON_STANDARD
#warning "This header is not standards compliant, please keep that in mind. use #define __LIBC_USE_NON_STANDARD to suppress this warning"
#endif

bool is_arg(char* in, char* arg, char* out);