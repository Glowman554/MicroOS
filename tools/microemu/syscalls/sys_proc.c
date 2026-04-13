#include <emu.h>
#include <scheduler.h>

void sys_spawn(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    int pid = sched_spawn(uc, ebx, ecx, edx);
    uint32_t result = (uint32_t)pid;
    uc_reg_write(uc, UC_X86_REG_ESI, &result);
}

void sys_get_proc_info(uc_engine *uc, uint32_t ebx) {
    emu_proc_t* proc = sched_get_proc(ebx);
    uint32_t exists = proc ? 1 : 0;
    uc_reg_write(uc, UC_X86_REG_ECX, &exists);
}

void sys_kill(uc_engine *uc, uint32_t ebx) {
    emu_proc_t *proc = sched_get_proc(ebx);
    if (proc) {
        proc->active = false;
        proc->stopped = true;
        proc->exit_code = -2;
        uc_emu_stop(proc->uc);
    }
    (void)uc;
}

void sys_get_exit_code(uc_engine *uc, uint32_t ebx) {
    int code = sched_get_exit_code(ebx);
    uint32_t result = (uint32_t)code;
    uc_reg_write(uc, UC_X86_REG_ECX, &result);
}
