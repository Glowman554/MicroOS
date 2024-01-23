#include <vm.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void microos();
void microos_sys();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <flbb> ...\n", argv[0]);
        return -1;
    }

    microos();
    microos_sys();

    struct vm_instance* vm = vm_load(argv[1]);
    
    stack_push(vm, argc - 1);
    stack_push(vm, (int64_t)&argv[1]);
    invoke(vm, vm->spark);

    vm_destroy(vm);
    return 0;
}