#include <gdb/gdb.h>

#include <driver/output/serial.h>
#include <utils/mmio.h>

gdb_state_t gdb_global_state = { 0 };

// TODO: check if memory is mapped before reading / writing
int gdb_sys_mem_readb(gdb_state_t* state, address addr, char* val) {
    *val = mmio_read8(addr);
    return 0;
}

int gdb_sys_mem_writeb(gdb_state_t* state, address addr, char val) {
    mmio_write8(addr, val);
    return 0;
}

int gdb_sys_continue(gdb_state_t* state) {
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] &= ~(1 << 8);
    return 0;
}

int gdb_sys_step(gdb_state_t* state) {
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] |= 1 << 8;
    return 0;
}

void gdb_interrupt(cpu_registers_t* regs) {
    switch (regs->intr) {
        case 1:
            gdb_global_state.signum = 5;
            break;
        case 3:
            gdb_global_state.signum = 5;
            break;
        default:
            gdb_global_state.signum = 7;
            break;
    }

    gdb_global_state.registers[GDB_CPU_I386_REG_EAX] = regs->eax;
    gdb_global_state.registers[GDB_CPU_I386_REG_ECX] = regs->ecx;
    gdb_global_state.registers[GDB_CPU_I386_REG_EDX] = regs->edx;
    gdb_global_state.registers[GDB_CPU_I386_REG_EBX] = regs->ebx;
    gdb_global_state.registers[GDB_CPU_I386_REG_ESP] = regs->esp;
    gdb_global_state.registers[GDB_CPU_I386_REG_EBP] = regs->ebp;
    gdb_global_state.registers[GDB_CPU_I386_REG_ESI] = regs->esi;
    gdb_global_state.registers[GDB_CPU_I386_REG_EDI] = regs->edi;
    gdb_global_state.registers[GDB_CPU_I386_REG_PC] = regs->eip;
    gdb_global_state.registers[GDB_CPU_I386_REG_CS] = regs->cs;
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] = regs->eflags;
    gdb_global_state.registers[GDB_CPU_I386_REG_SS] = regs->ss;
    gdb_global_state.registers[GDB_CPU_I386_REG_DS] = regs->ds;
    gdb_global_state.registers[GDB_CPU_I386_REG_ES] = regs->es;
    gdb_global_state.registers[GDB_CPU_I386_REG_FS] = regs->fs;
    gdb_global_state.registers[GDB_CPU_I386_REG_GS] = regs->gs;

    gdb_main(&gdb_global_state);

    regs->eax = gdb_global_state.registers[GDB_CPU_I386_REG_EAX];
    regs->ecx = gdb_global_state.registers[GDB_CPU_I386_REG_ECX];
    regs->edx = gdb_global_state.registers[GDB_CPU_I386_REG_EDX];
    regs->ebx = gdb_global_state.registers[GDB_CPU_I386_REG_EBX];
    regs->esp = gdb_global_state.registers[GDB_CPU_I386_REG_ESP];
    regs->ebp = gdb_global_state.registers[GDB_CPU_I386_REG_EBP];
    regs->esi = gdb_global_state.registers[GDB_CPU_I386_REG_ESI];
    regs->edi = gdb_global_state.registers[GDB_CPU_I386_REG_EDI];
    regs->eip = gdb_global_state.registers[GDB_CPU_I386_REG_PC];
    regs->cs = gdb_global_state.registers[GDB_CPU_I386_REG_CS];
    regs->eflags = gdb_global_state.registers[GDB_CPU_I386_REG_PS];
    regs->ss = gdb_global_state.registers[GDB_CPU_I386_REG_SS];
    regs->ds = gdb_global_state.registers[GDB_CPU_I386_REG_DS];
    regs->es = gdb_global_state.registers[GDB_CPU_I386_REG_ES];
    regs->fs = gdb_global_state.registers[GDB_CPU_I386_REG_FS];
    regs->gs = gdb_global_state.registers[GDB_CPU_I386_REG_GS];
}