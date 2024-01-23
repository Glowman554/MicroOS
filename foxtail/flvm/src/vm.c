#include <vm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

struct vm_native* natives = NULL;
int num_natives = 0;


void stack_push(struct vm_instance* vm, int64_t value) {
    // printf("push: %d\n", vm->stack_ptr);
    vm->stack[vm->stack_ptr++] = value;
}

int64_t stack_pop(struct vm_instance* vm) {
    // printf("pop: %d\n", vm->stack_ptr);
    return vm->stack[--vm->stack_ptr];
}

void nativePrintc(struct vm_instance* vm) {
    char c = stack_pop(vm);
    putchar(c);
    stack_push(vm, 0);
}

void nativeAllocate(struct vm_instance* vm) {
    stack_push(vm, (int64_t)malloc(stack_pop(vm)));
}

void nativeDeallocate(struct vm_instance* vm) {
    free((void*)stack_pop(vm));
    stack_push(vm, 0);
}

void nativeDoExit(struct vm_instance* vm) {
    exit(stack_pop(vm));
}

void nativeFileOpen(struct vm_instance* vm) {
    const char* mode = (const char*)stack_pop(vm);
    const char* file = (const char*)stack_pop(vm);
    stack_push(vm, (int64_t)fopen(file, mode));
}

void nativeFileWrite(struct vm_instance* vm) {
    int64_t offset = stack_pop(vm);
    int64_t size = stack_pop(vm);
    const char* buffer = (const char*)stack_pop(vm);
    FILE* f = (FILE*)stack_pop(vm);

    fseek(f, offset, SEEK_SET);
    fwrite(buffer, size, 1, f);
    stack_push(vm, 0);
}

void nativeFileRead(struct vm_instance* vm) {
    int64_t offset = stack_pop(vm);
    int64_t size = stack_pop(vm);
    char* buffer = (char*)stack_pop(vm);
    FILE* f = (FILE*)stack_pop(vm);

    fseek(f, offset, SEEK_SET);
    fread(buffer, size, 1, f);
    stack_push(vm, 0);
}

void nativeFileClose(struct vm_instance* vm) {
    fclose((FILE*)stack_pop(vm));
    stack_push(vm, 0);
}

void nativeFileSize(struct vm_instance* vm) {
    FILE* f = (FILE*)stack_pop(vm);
    fseek(f, 0, SEEK_END);
    stack_push(vm, ftell(f));
}

void nativeMemoryWrite16(struct vm_instance* vm) {
    int64_t value = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    *(uint16_t*)ptr = value;
    stack_push(vm, 0);
}

void nativeMemoryRead16(struct vm_instance* vm) {
    stack_push(vm, *(uint16_t*)stack_pop(vm));
}

void nativeMemoryWrite32(struct vm_instance* vm) {
    int64_t value = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    *(uint32_t*)ptr = value;
    stack_push(vm, 0);
}

void nativeMemoryRead32(struct vm_instance* vm) {
    stack_push(vm, *(uint32_t*)stack_pop(vm));
}

void nativeCall0(struct vm_instance* vm) {
    int64_t ptr = stack_pop(vm);
    invoke(vm, ptr);
}

void nativeCall1(struct vm_instance* vm) {
    int64_t arg1 = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    stack_push(vm, arg1);
    invoke(vm, ptr);
}

void nativeCall2(struct vm_instance* vm) {
    int64_t arg2 = stack_pop(vm);
    int64_t arg1 = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    stack_push(vm, arg1);
    stack_push(vm, arg2);
    invoke(vm, ptr);
}

void nativeCall3(struct vm_instance* vm) {
    int64_t arg3 = stack_pop(vm);
    int64_t arg2 = stack_pop(vm);
    int64_t arg1 = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    stack_push(vm, arg1);
    stack_push(vm, arg2);
    stack_push(vm, arg3);
    invoke(vm, ptr);
}

void nativeCall4(struct vm_instance* vm) {
    int64_t arg4 = stack_pop(vm);
    int64_t arg3 = stack_pop(vm);
    int64_t arg2 = stack_pop(vm);
    int64_t arg1 = stack_pop(vm);
    int64_t ptr = stack_pop(vm);
    stack_push(vm, arg1);
    stack_push(vm, arg2);
    stack_push(vm, arg3);
    stack_push(vm, arg4);
    invoke(vm, ptr);
}


NativeFunction nativeFunctions[] = {
    nativePrintc,
    nativeAllocate,
    nativeDeallocate,
    nativeDoExit,
    nativeFileOpen,
    nativeFileWrite,
    nativeFileRead,
    nativeFileClose,
    nativeFileSize,
    nativeMemoryWrite16,
    nativeMemoryRead16,
    nativeMemoryWrite32,
    nativeMemoryRead32,
    nativeCall0,
    nativeCall1,
    nativeCall2,
    nativeCall3,
    nativeCall4
};


void resizeGlobalsIfNecessary(struct vm_instance* vm, int idx) {
    idx++;
    if (idx > vm->global_variable_size) {
        vm->global_variables = realloc(vm->global_variables, sizeof(int64_t) * idx);
        vm->global_variable_types = realloc(vm->global_variable_types, sizeof(uint8_t) * idx);
        vm->global_variable_size = idx;
    }
}

int64_t* selectVariable(uint64_t idx, int64_t* local, int64_t* global) {
    if (idx >= 256) {
        return &global[idx - 256];
    }
    else {
        return &local[idx];
    }
}

uint8_t getVariableType(uint64_t idx, uint8_t* local, uint8_t* global) {
    if (idx >= 256) {
        return global[idx - 256] & ~(1 << 7);
    }
    else {
        return local[idx] & ~(1 << 7);
    }
}

bool getVariableArray(uint64_t idx, uint8_t* local, uint8_t* global) {
    if (idx >= 256) {
        return !(global[idx - 256] & (1 << 7));
    }
    else {
        return !(local[idx] & (1 << 7));
    }
}

void invoke(struct vm_instance* vm, uint64_t location) {
    uint64_t counter = location;

    int64_t variables[256] = { 0 };
    uint8_t variable_types[256] = { 0 };

    bool noreturn = false;

    while (true) {
        uint8_t instruction = read_u8(vm->code, counter++);

        switch (instruction) {
        case GLOBAL_RESERVE:
        case VARIABLE:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;
            uint8_t datatype = read_u8(vm->code, counter++);
            uint8_t array = read_u8(vm->code, counter++);
            if (!array) {
                datatype |= (1 << 7);
            }

            if (instruction == GLOBAL_RESERVE) {
                idx -= 256;
                resizeGlobalsIfNecessary(vm, idx);
                vm->global_variables[idx] = 0;
                vm->global_variable_types[idx] = datatype;
            }
            else {
                variables[idx] = 0;
                variable_types[idx] = datatype;
            }
        }
        break;

        case ASSIGN:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;
            *selectVariable(idx, variables, vm->global_variables) = stack_pop(vm);
        }
        break;
        case ASSIGN_INDEXED:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;
            int64_t b = stack_pop(vm);
            int64_t a = stack_pop(vm);
            if (getVariableType(idx, variable_types, vm->global_variable_types) == CHR) {
                write_i8(*selectVariable(idx, variables, vm->global_variables), a, b);
            }
            else {
                write_i64(*selectVariable(idx, variables, vm->global_variables), a * 8, b);
            }
        }
        break;
        case LOAD:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;
            stack_push(vm, *selectVariable(idx, variables, vm->global_variables));
        }
        break;
        case LOAD_INDEXED:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;
            if (getVariableArray(idx, variable_types, vm->global_variable_types)) {
                if (getVariableType(idx, variable_types, vm->global_variable_types) == CHR) {
                    stack_push(vm, read_i8(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm)));
                }
                else {
                    stack_push(vm, read_i64(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm) * 8));
                }
            }
            else {
                stack_push(vm, (*selectVariable(idx, variables, vm->global_variables) & (1 << stack_pop(vm))) ? 1 : 0);
            }
        }
        break;

        case STRING:
        {
            uint64_t len = read_u64(vm->code, counter);
            counter += 8;

            stack_push(vm, (int64_t)vm->code + counter);

            counter += len + 1;
        }
        break;
        case NUMBER:
        {
            int64_t num = read_u64(vm->code, counter);
            counter += 8;
            stack_push(vm, num);
        }
        break;

        case GOTO:
        {
            counter = read_u64(vm->code, counter);
        }
        break;
        case GOTO_TRUE:
        case GOTO_FALSE:
        {
            uint64_t nextCounter = read_u64(vm->code, counter);
            counter += 8;

            if (instruction == GOTO_TRUE) {
                if (stack_pop(vm)) {
                    counter = nextCounter;
                }
            }
            else {
                if (!stack_pop(vm)) {
                    counter = nextCounter;
                }
            }
        }
        break;
        case INVOKE:
        {
            uint64_t loc = read_u64(vm->code, counter);
            counter += 8;

            invoke(vm, loc);
        }
        break;
        case INVOKE_NATIVE:
        {
            uint64_t nativeID = read_u64(vm->code, counter);
            counter += 8;

            if (nativeID >= sizeof(nativeFunctions) / sizeof(NativeFunction)) {
                for (int i = 0; i < num_natives; i++) {
                    if (natives[i].id == nativeID) {
                        natives[i].function(vm);
                        goto done;
                    }
                }
                printf("Native with id %ld not found!\n", nativeID);
                abort();
            done:;
            } else {
                nativeFunctions[nativeID](vm);
            }
        }
        break;
        case RETURN:
        {
            if (noreturn) {
                invoke(vm, read_u64(vm->code, 16));
            }
            return;
        }
        break;

        case INCREASE:
        case DECREASE:
        {
            uint64_t idx = read_u64(vm->code, counter);
            counter += 8;

            if (instruction == INCREASE) {
                *selectVariable(idx, variables, vm->global_variables) += 1;
            }
            else {
                *selectVariable(idx, variables, vm->global_variables) -= 1;
            }
        }
        break;

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case MOD:
        case LESS:
        case LESS_EQUALS:
        case MORE:
        case MORE_EQUALS:
        case EQUALS:
        case NOT_EQUALS:
        case SHIFT_LEFT:
        case SHIFT_RIGHT:
        case OR:
        case AND:
        case XOR:
        {
            int64_t b = stack_pop(vm);
            int64_t a = stack_pop(vm);
            switch (instruction) {
            case ADD:
                stack_push(vm, a + b);
                break;
            case SUB:
                stack_push(vm, a - b);
                break;
            case MUL:
                stack_push(vm, a * b);
                break;
            case DIV:
                stack_push(vm, a / b);
                break;
            case MOD:
                stack_push(vm, a % b);
                break;

            case LESS:
                stack_push(vm, a < b);
                break;
            case LESS_EQUALS:
                stack_push(vm, a <= b);
                break;
            case MORE:
                stack_push(vm, a > b);
                break;
            case MORE_EQUALS:
                stack_push(vm, a >= b);
                break;
            case EQUALS:
                stack_push(vm, a == b);
                break;
            case NOT_EQUALS:
                stack_push(vm, a != b);
                break;

            case SHIFT_LEFT:
                stack_push(vm, a << b);
                break;
            case SHIFT_RIGHT:
                stack_push(vm, a >> b);
                break;
            case OR:
                stack_push(vm, a | b);
                break;
            case AND:
                stack_push(vm, a & b);
                break;
            case XOR:
                stack_push(vm, a ^ b);
                break;
            }
        }
        break;


        case INVERT:
        {
            stack_push(vm, !stack_pop(vm));
        }
        break;


        case NOT:
        {
            stack_push(vm, ~stack_pop(vm));
        }
        break;

        case NORETURN:
        {
            noreturn = true;
        }
        break;

        case DELETE:
        {
            stack_pop(vm);
        }
        break;

        default:
            printf("Invalid instruction %d\n", instruction);
            abort();
        }
    }
}

#define STACK 256
struct vm_instance* vm_load(const char* file) {
    struct vm_instance* vm = malloc(sizeof(struct vm_instance));
    memset(vm, 0, sizeof(struct vm_instance));

    vm->stack = malloc(sizeof(int64_t) * STACK);
    vm->max_stack = STACK;
    
    FILE* codeFile = fopen(file, "rb");
    assert(codeFile);

    fseek(codeFile, 0, SEEK_END);
    int codeSize = ftell(codeFile);
    fseek(codeFile, 0, SEEK_SET);

    vm->code = malloc(codeSize);
    fread(vm->code, codeSize, 1, codeFile);
    fclose(codeFile);

    invoke(vm, read_i64(vm->code, 8)); // globals
    vm->spark = read_i64(vm->code, 0);

    return vm;
}

void vm_destroy(struct vm_instance* vm) {
    free(vm->code);
    free(vm->stack);
    
    if (vm->global_variables) {
        free(vm->global_variables);
    }
    if (vm->global_variable_types) {
        free(vm->global_variable_types);
    }
    free(vm);
}

void vm_native_register(int id, NativeFunction function) {
    natives = realloc(natives, sizeof(struct vm_native) * (num_natives + 1));
    natives[num_natives].function = function;
    natives[num_natives].id = id;
    num_natives++;
}