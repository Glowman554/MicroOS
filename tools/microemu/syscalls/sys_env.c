#include <emu.h>
#include <string.h>
#include <scheduler.h>

void sys_env(uc_engine *uc, uint32_t ebx, uint32_t ecx) {
    emu_proc_t* current = sched_current();

    int id = ebx;
    switch (id) {
        case SYS_GET_ARGV_ID:
            uc_reg_write(uc, UC_X86_REG_ECX, &current->argv_addr);
            break;
        case SYS_GET_ENVP_ID:
            uc_reg_write(uc, UC_X86_REG_ECX, &current->envp_addr);
            break;
        case SYS_GET_PWD_ID:
            uc_mem_write(uc, ecx, current->pwd, strlen(current->pwd) + 1);
            break;
        case SYS_SET_PWD_ID:
            read_emu_string(uc, ecx, current->pwd, sizeof(current->pwd));
            break;
        case SYS_PWR_RESET_ID:
            break;
        case SYS_PWR_SHUTDOWN_ID:
            exit(0);
            break;
        case SYS_ENV_PIN:
            break;
        case SYS_ENV_SET_LAYOUT:
            break;
        case SYS_ENV_TASK_SET_WAIT_TIME:
            break;
        default:
            fatalf("[emu] unknown env syscall %d\n", id);
            break;
    }
}
