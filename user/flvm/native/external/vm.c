#include <native.h>

void native_stack_push(struct vm_instance* vm) {
    int64_t value = (int64_t) stack_pop(vm);
    struct vm_instance* vm2 = (struct vm_instance*) stack_pop(vm);
    stack_push(vm2, value);
    stack_push(vm, 0);
}

void native_stack_pop(struct vm_instance* vm) {
    struct vm_instance* vm2 = (struct vm_instance*) stack_pop(vm);
    stack_push(vm, stack_pop(vm2));
}

void native_invoke(struct vm_instance* vm) {
    uint64_t location = (uint64_t) stack_pop(vm);
    struct vm_instance* vm2 = (struct vm_instance*) stack_pop(vm);
    invoke(vm2, location);
    stack_push(vm, 0);
}

void native_vm_load(struct vm_instance* vm) {
    const char* file = (const char*) stack_pop(vm);
    stack_push(vm, (int64_t) vm_load(file));
}

void native_vm_destroy(struct vm_instance* vm) {
    struct vm_instance* vm2 = (struct vm_instance*) stack_pop(vm);
    vm_destroy(vm2);
    stack_push(vm, 0);
}

void native_vm() {
    vm_native_register(48327, native_stack_push);
    vm_native_register(48328, native_stack_pop);
    vm_native_register(48329, native_invoke);
    vm_native_register(48330, native_vm_load);
    vm_native_register(48331, native_vm_destroy);
}