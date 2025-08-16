#include <flvm.h>

#include <stdint.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <flbb> ...\n", argv[0]);
        return -1;
    }

    struct vm_instance* vm = vm_load(argv[1]);
    
    stack_push(vm, argc - 1);
    stack_push(vm, (int64_t)(uint32_t)&argv[1]);
    invoke(vm, vm->spark);

    vm_destroy(vm);
}