#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <interrupts/interrupts.h>

typedef uint32_t address;
typedef uint32_t reg;

enum GDB_REGISTER {
    GDB_CPU_I386_REG_EAX  = 0,
    GDB_CPU_I386_REG_ECX  = 1,
    GDB_CPU_I386_REG_EDX  = 2,
    GDB_CPU_I386_REG_EBX  = 3,
    GDB_CPU_I386_REG_ESP  = 4,
    GDB_CPU_I386_REG_EBP  = 5,
    GDB_CPU_I386_REG_ESI  = 6,
    GDB_CPU_I386_REG_EDI  = 7,
    GDB_CPU_I386_REG_PC   = 8,
    GDB_CPU_I386_REG_PS   = 9,
    GDB_CPU_I386_REG_CS   = 10,
    GDB_CPU_I386_REG_SS   = 11,
    GDB_CPU_I386_REG_DS   = 12,
    GDB_CPU_I386_REG_ES   = 13,
    GDB_CPU_I386_REG_FS   = 14,
    GDB_CPU_I386_REG_GS   = 15,
    GDB_CPU_NUM_REGISTERS = 16
};

typedef struct gdb_state {
    int signum;
    reg registers[GDB_CPU_NUM_REGISTERS];
} gdb_state_t;

extern bool gdb_active;

#define GDB_EOF (-1)
#define gdb_is_printable_char(ch) (ch >= 0x20 && ch <= 0x7e)

int gdb_main(gdb_state_t* state);
void gdb_interrupt(cpu_registers_t* regs);

int gdb_sys_mem_readb(gdb_state_t* state, address addr, char* val);
int gdb_sys_mem_writeb(gdb_state_t* state, address addr, char val);
int gdb_sys_continue(gdb_state_t* state);
int gdb_sys_step(gdb_state_t* state);


int gdb_get_val(char digit, int base);
int gdb_strtol(const char* str, unsigned int len, int base, const char** endptr);

int gdb_enc_hex(char* buf, int buf_len, const char* data, int data_len);
int gdb_dec_hex(const char* buf,  int buf_len, char* data, int data_len);
int gdb_enc_bin(char* buf, int buf_len, const char* data, int data_len);
int gdb_dec_bin(const char* buf,  int buf_len, char* data, int data_len);

int gdb_sys_getc(gdb_state_t* state);
int gdb_sys_putchar(gdb_state_t* state, int ch);
int gdb_write(gdb_state_t* state, const char* buf, int len);
int gdb_read(gdb_state_t* state, char* buf, int buf_len, int len);