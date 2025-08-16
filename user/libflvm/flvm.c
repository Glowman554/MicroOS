#include <flvm.h>

#include <stdint.h>
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

void nativeExit(struct vm_instance* vm) {
    int c = stack_pop(vm);
    exit(c);
    stack_push(vm, 0);
}

void nativePutchar(struct vm_instance* vm) {
    int c = stack_pop(vm);
    putchar(c);
    stack_push(vm, 0);
}

void nativePuts(struct vm_instance* vm) {
    char* s = (char*) (uint32_t) stack_pop(vm);
    puts(s);
    stack_push(vm, 0);
}

void nativeMalloc(struct vm_instance* vm) {
    stack_push(vm, (int64_t)(uint32_t)malloc(stack_pop(vm)));
}

void nativeFree(struct vm_instance* vm) {
    free((void*)(uint32_t)stack_pop(vm));
    stack_push(vm, 0);
}

void nativeFopen(struct vm_instance* vm) {
    const char* mode = (const char*)(uint32_t)stack_pop(vm);
    const char* file = (const char*)(uint32_t)stack_pop(vm);
    stack_push(vm, (int64_t)(uint32_t)fopen(file, mode));
}

void nativeFclose(struct vm_instance* vm) {
    fclose((FILE*)(uint32_t)stack_pop(vm));
    stack_push(vm, 0);
}

void nativeFseek(struct vm_instance* vm) {
    int64_t whence = stack_pop(vm);
    int64_t offset = stack_pop(vm);
    FILE* f = (FILE*)(uint32_t)stack_pop(vm);

    fseek(f, offset, whence);
    stack_push(vm, 0);
}

void nativeFread(struct vm_instance* vm) {
    FILE* f = (FILE*)(uint32_t)stack_pop(vm);
    int64_t n = stack_pop(vm);
    int64_t size = stack_pop(vm);
    char* buffer = (char*)(uint32_t)stack_pop(vm);

    stack_push(vm, fread(buffer, size, n, f));
}

void nativeFwrite(struct vm_instance* vm) {
    FILE* f = (FILE*)stack_pop(vm);
    int64_t n = stack_pop(vm);
    int64_t size = stack_pop(vm);
    char* buffer = (char*)(uint32_t)stack_pop(vm);

    stack_push(vm, fwrite(buffer, size, n, f));
}

void nativeFtell(struct vm_instance* vm) {
    FILE* f = (FILE*)(uint32_t)stack_pop(vm);
    stack_push(vm, ftell(f));
}


NativeFunction nativeFunctions[] = {
    nativeExit,
    nativePutchar,
    nativePuts,
    nativeMalloc,
    nativeFree,
    nativeFopen,
    nativeFclose,
    nativeFseek,
    nativeFread,
    nativeFwrite,
    nativeFtell
};


void resizeGlobalsIfNecessary(struct vm_instance* vm, int idx) {
    idx++;
    if (idx > vm->global_variable_size) {
        vm->global_variables = realloc(vm->global_variables, sizeof(int64_t) * idx);
        vm->global_variable_types = realloc(vm->global_variable_types, sizeof(uint8_t) * idx);
        vm->global_variable_size = idx;
    }
}

int datatypeToSize(int dt) {
    switch (dt) {
    case INT:
        return 8;
    case CHR:
        return 1;
    case STR:
        return 8;
    case PTR:
        return 8;
    case I16:
        return 2;
    case I32:
        return 4;
    default:
        printf("Invalid datatype %d", dt);
        abort();
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
            int sz = datatypeToSize(getVariableType(idx, variable_types, vm->global_variable_types));
            switch (sz) {
                case 1:
                    write_i8(*selectVariable(idx, variables, vm->global_variables), a, b);
                    break;
                case 2:
                    write_i16(*selectVariable(idx, variables, vm->global_variables), a * 2, b);
                    break;
                case 4:
                    write_i32(*selectVariable(idx, variables, vm->global_variables), a * 4, b);
                    break;
                case 8:
                    write_i64(*selectVariable(idx, variables, vm->global_variables), a * 8, b);
                    break;
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
                int sz = datatypeToSize(getVariableType(idx, variable_types, vm->global_variable_types));
                switch (sz) {
                    case 1:
                        stack_push(vm, (uint8_t) read_i8(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm)));
                        break;
                    case 2:
                        stack_push(vm, (uint16_t) read_i16(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm) * 2));
                        break;
                    case 4:
                        stack_push(vm, (uint32_t) read_i32(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm) * 4));
                        break;
                    case 8:
                        stack_push(vm, read_i64(*selectVariable(idx, variables, vm->global_variables), stack_pop(vm) * 8));
                        break;
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

        case CHANGE_SIGN:
        {
            stack_push(vm, -stack_pop(vm));
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