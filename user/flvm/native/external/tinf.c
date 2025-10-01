#include <native.h>
#include <tinf.h>

void native_tinf_init(struct vm_instance* vm) {
    tinf_init();
    stack_push(vm, 0);
}

void native_tinf_uncompress(struct vm_instance* vm) {
    unsigned int sourceLen = (unsigned int) stack_pop(vm);
    const void* source = (const void*) (uint32_t) stack_pop(vm);
    unsigned int* destLen = (unsigned int*) (uint32_t) stack_pop(vm);
    void* dest = (void*) (uint32_t) stack_pop(vm);
    stack_push(vm, tinf_uncompress(dest, destLen, source, sourceLen));
}

void native_tinf_gzip_uncompress(struct vm_instance* vm) {
    unsigned int sourceLen = (unsigned int) stack_pop(vm);
    const void* source = (const void*) (uint32_t) stack_pop(vm);
    unsigned int* destLen = (unsigned int*) (uint32_t) stack_pop(vm);
    void* dest = (void*) (uint32_t) stack_pop(vm);
    stack_push(vm, tinf_gzip_uncompress(dest, destLen, source, sourceLen));
}

void native_tinf_zlib_uncompress(struct vm_instance* vm) {
    unsigned int sourceLen = (unsigned int) stack_pop(vm);
    const void* source = (const void*) (uint32_t) stack_pop(vm);
    unsigned int* destLen = (unsigned int*) (uint32_t) stack_pop(vm);
    void* dest = (void*) (uint32_t) stack_pop(vm);
    stack_push(vm, tinf_zlib_uncompress(dest, destLen, source, sourceLen));
}

void native_tinf() {
    vm_native_register(35269, native_tinf_init);
    vm_native_register(35270, native_tinf_uncompress);
    vm_native_register(35271, native_tinf_gzip_uncompress);
    vm_native_register(35272, native_tinf_zlib_uncompress);
}