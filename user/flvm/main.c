#include <flvm.h>

#include <load.h>

#include <stdint.h>
#include <non-standart/stdio.h>
#include <stdlib.h>

void native_extension(struct vm_instance* vm) {
    char* s = (char*) (uint32_t) stack_pop(vm);

    printf("Loading extension %s\n", s);

	FILE* file = fopen(s, "r");
	if (!file) {
		printf("Could not open %s\n", s);
		abort();
	}

	fsize(file, size);
	void* buf = malloc(size);
	fread(buf, 1, size, file);
	fclose(file);
    

    loaded_object_t obj = load(buf, size);
    
    void (*init)() = symbol(&obj, "init");
	if (!init) {
		printf("Could not find init symbol\n");
        abort();
	}

    init();

    stack_push(vm, 0);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <flbb> ...\n", argv[0]);
        return -1;
    }

    struct vm_instance* vm = vm_load(argv[1]);
    
    vm_native_register(70894354, native_extension);

    stack_push(vm, argc - 1);
    stack_push(vm, (int64_t)(uint32_t)&argv[1]);
    invoke(vm, vm->spark);

    vm_destroy(vm);

    return 0;
}