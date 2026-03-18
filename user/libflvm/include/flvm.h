#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_CALL_DEPTH 256

struct call_frame {
    uint64_t return_address;
    int64_t variables[256];
    uint8_t variable_types[256];
    bool noreturn;
};

struct vm_instance {
	void* code;

	int64_t* stack;
	int stack_ptr;
	int max_stack;

	int64_t* global_variables;
	uint8_t* global_variable_types;
	int global_variable_size;

    uint64_t counter;
    struct call_frame call_stack[MAX_CALL_DEPTH];
    int call_depth;
    bool running;

    int64_t globals;
    int64_t spark;
    int64_t code_size;
};

typedef void (*NativeFunction)(struct vm_instance* vm);

struct vm_native {
    NativeFunction function;
    int id;
};

extern struct vm_native* natives;
extern int num_natives;

enum Instructions {
    GLOBAL_RESERVE,

    ASSIGN,
    ASSIGN_INDEXED,
    LOAD,
    LOAD_INDEXED,

    NUMBER,
    STRING,

    GOTO,
    GOTO_TRUE,
    GOTO_FALSE,
    INVOKE,
    INVOKE_NATIVE,
    RETURN,

    VARIABLE,

    INCREASE,
    DECREASE,

    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    LESS,
    LESS_EQUALS,
    MORE,
    MORE_EQUALS,
    EQUALS,
    NOT_EQUALS,

    INVERT,

    SHIFT_LEFT,
    SHIFT_RIGHT,
    OR,
    AND,
    XOR,
    NOT,


    NORETURN,
    DELETE,

    CHANGE_SIGN
};

enum Datatypes {
    INT,
    CHR,
    STR,
    PTR,
    I16,
    I32
};

#define read_i64(ptr, offset) *((int64_t*)((int64_t) ptr + offset))
#define write_i64(ptr, offset, value) *((int64_t*)((int64_t) ptr + offset)) = value

#define read_u64(ptr, offset) *((uint64_t*)((int64_t) ptr + offset))

#define read_u8(ptr, offset) *((uint8_t*)((int64_t) ptr + offset))
#define write_u8(ptr, offset, value) *((uint8_t*)((int64_t) ptr + offset)) = value

#define read_i8(ptr, offset) *((int8_t*)((int64_t) ptr + offset))
#define write_i8(ptr, offset, value) *((int8_t*)((int64_t) ptr + offset)) = value

#define read_i16(ptr, offset) *((int16_t*)((int64_t) ptr + offset))
#define write_i16(ptr, offset, value) *((int16_t*)((int64_t) ptr + offset)) = value

#define read_i32(ptr, offset) *((int32_t*)((int64_t) ptr + offset))
#define write_i32(ptr, offset, value) *((int32_t*)((int64_t) ptr + offset)) = value


void stack_push(struct vm_instance* vm, int64_t value);
int64_t stack_pop(struct vm_instance* vm);


void invoke(struct vm_instance* vm, uint64_t location);
void step(struct vm_instance* vm);

struct vm_instance* vm_load(const char* file);
struct vm_instance* vm_create(void* code, int64_t code_size);
void vm_destroy(struct vm_instance* vm);

void vm_native_register(int id, NativeFunction function);