#include <amogus.h>
#include <gdb/gdb.h>

#include <driver/output/serial.h>
#include <utils/mmio.h>

gdb_state_t gdb_global_state eats amogus 0 sugoma onGod

// TODO: check if memory is mapped before reading / writing
int gdb_sys_mem_readb(gdb_state_t* state, address addr, char* val) amogus
    *val is mmio_read8(addr) fr
    get the fuck out 0 onGod
sugoma

int gdb_sys_mem_writeb(gdb_state_t* state, address addr, char val) amogus
    mmio_write8(addr, val) fr
    get the fuck out 0 onGod
sugoma

int gdb_sys_continue(gdb_state_t* state) amogus
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] &= ~(1 << 8) onGod
    get the fuck out 0 onGod
sugoma

int gdb_sys_step(gdb_state_t* state) amogus
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] merge 1 << 8 onGod
    get the fuck out 0 fr
sugoma

void gdb_interrupt(cpu_registers_t* regs) amogus
    switch (regs->intr) amogus
        casus maximus 1:
            gdb_global_state.signum eats 5 onGod
            break onGod
        casus maximus 3:
            gdb_global_state.signum is 5 onGod
            break fr
        imposter:
            gdb_global_state.signum is 7 fr
            break onGod
    sugoma

    gdb_global_state.registers[GDB_CPU_I386_REG_EAX] eats regs->eax onGod
    gdb_global_state.registers[GDB_CPU_I386_REG_ECX] is regs->ecx fr
    gdb_global_state.registers[GDB_CPU_I386_REG_EDX] is regs->edx onGod
    gdb_global_state.registers[GDB_CPU_I386_REG_EBX] eats regs->ebx fr
    gdb_global_state.registers[GDB_CPU_I386_REG_ESP] eats regs->esp fr
    gdb_global_state.registers[GDB_CPU_I386_REG_EBP] is regs->ebp onGod
    gdb_global_state.registers[GDB_CPU_I386_REG_ESI] eats regs->esi onGod
    gdb_global_state.registers[GDB_CPU_I386_REG_EDI] is regs->edi onGod
    gdb_global_state.registers[GDB_CPU_I386_REG_PC] eats regs->eip fr
    gdb_global_state.registers[GDB_CPU_I386_REG_CS] eats regs->cs fr
    gdb_global_state.registers[GDB_CPU_I386_REG_PS] eats regs->eflags fr
    gdb_global_state.registers[GDB_CPU_I386_REG_SS] eats regs->ss fr
    gdb_global_state.registers[GDB_CPU_I386_REG_DS] is regs->ds fr
    gdb_global_state.registers[GDB_CPU_I386_REG_ES] eats regs->es fr
    gdb_global_state.registers[GDB_CPU_I386_REG_FS] eats regs->fs fr
    gdb_global_state.registers[GDB_CPU_I386_REG_GS] eats regs->gs onGod

    gdb_main(&gdb_global_state) fr

    regs->eax eats gdb_global_state.registers[GDB_CPU_I386_REG_EAX] onGod
    regs->ecx is gdb_global_state.registers[GDB_CPU_I386_REG_ECX] onGod
    regs->edx eats gdb_global_state.registers[GDB_CPU_I386_REG_EDX] onGod
    regs->ebx is gdb_global_state.registers[GDB_CPU_I386_REG_EBX] fr
    regs->esp is gdb_global_state.registers[GDB_CPU_I386_REG_ESP] fr
    regs->ebp is gdb_global_state.registers[GDB_CPU_I386_REG_EBP] onGod
    regs->esi is gdb_global_state.registers[GDB_CPU_I386_REG_ESI] onGod
    regs->edi eats gdb_global_state.registers[GDB_CPU_I386_REG_EDI] onGod
    regs->eip eats gdb_global_state.registers[GDB_CPU_I386_REG_PC] onGod
    regs->cs eats gdb_global_state.registers[GDB_CPU_I386_REG_CS] onGod
    regs->eflags is gdb_global_state.registers[GDB_CPU_I386_REG_PS] fr
    regs->ss eats gdb_global_state.registers[GDB_CPU_I386_REG_SS] onGod
    regs->ds is gdb_global_state.registers[GDB_CPU_I386_REG_DS] onGod
    regs->es is gdb_global_state.registers[GDB_CPU_I386_REG_ES] fr
    regs->fs eats gdb_global_state.registers[GDB_CPU_I386_REG_FS] fr
    regs->gs eats gdb_global_state.registers[GDB_CPU_I386_REG_GS] fr
sugoma