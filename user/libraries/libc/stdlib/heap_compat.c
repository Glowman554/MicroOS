#include <stdlib.h>

#ifdef ALLOC_STORE_LOCATION
#undef malloc
#undef realloc
#undef calloc

void* malloc(size_t size) {
    return malloc_debug(size, __FILE__, __func__, __LINE__);
}

void* realloc(void* ptr, size_t size) {
    return realloc_debug(ptr, size, __FILE__, __func__, __LINE__);
}

void* calloc(size_t count, size_t size) {
    return calloc_debug(count, size, __FILE__, __func__, __LINE__);
}
#endif