#include <emu.h>
#include <scheduler.h>
#include <fs/hostfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <mex.h>
#include <elf.h>

#include <stdlib.h>
#include <termios.h>


#define align_down(x, a) ((x) & ~((a) - 1))
#define align_up(x, a) (((x) + (a) - 1) & ~((a) - 1))

char emu_pwd[512] = "root:/";

struct termios orig_termios;
void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void read_emu_string(uc_engine *uc, uint32_t addr, char *buf, size_t max) {
    for (size_t i = 0; i < max - 1; i++) {
        uc_mem_read(uc, addr + i, &buf[i], 1);
        if (buf[i] == '\0') return;
    }
    buf[max - 1] = '\0';
}

void hook_intr(uc_engine *uc, uint32_t intno, void *user_data) {
    if (intno != 0x30) {
        return;
    }
    
    uint32_t eax, ebx, ecx, edx, esi, edi;
    uc_reg_read(uc, UC_X86_REG_EAX, &eax);
    uc_reg_read(uc, UC_X86_REG_EBX, &ebx);
    uc_reg_read(uc, UC_X86_REG_ECX, &ecx);
    uc_reg_read(uc, UC_X86_REG_EDX, &edx);
    uc_reg_read(uc, UC_X86_REG_ESI, &esi);
    uc_reg_read(uc, UC_X86_REG_EDI, &edi);
    
    switch (eax) {
        case SYS_OPEN_ID:
            sys_open(uc, ebx, ecx);
            break;
        case SYS_CLOSE_ID:
            sys_close(ebx);
            break;
        case SYS_READ_ID:
            sys_read(uc, ebx, ecx, edx, esi);
            break;
        case SYS_WRITE_ID:
            sys_write(uc, ebx, ecx, edx, esi);
            break;
        case SYS_FILESIZE_ID:
            sys_filesize(uc, ebx);
            break;
        case SYS_DELETE_ID:
            sys_delete(ebx);
            break;
        case SYS_MKDIR_ID:
            sys_mkdir(uc, ebx);
            break;
        case SYS_DIR_AT_ID:
            sys_dir_at(uc, ebx, ecx, edx);
            break;
        case SYS_TOUCH_ID:
            sys_touch(uc, ebx);
            break;
        case SYS_DELETE_DIR_ID:
            sys_delete_dir(uc, ebx);
            break;
        case SYS_FS_AT_ID:
            sys_fs_at(uc, ebx, ecx);
            break;
        case SYS_TRUNCATE_ID:
            sys_truncate(ebx, ecx);
            break;
        case SYS_ENV_ID:
            sys_env(uc, ebx, ecx);
            break;
        case SYS_MMAP_ID:
            sys_mmap(uc, ebx);
            break;
        case SYS_MMAP_MAPPED_ID:
            sys_mmap_mapped(uc, ebx);
            break;
        case SYS_ASYNC_GETC_ID:
            sys_async_getc(uc);
            break;
        case SYS_ASYNC_GETARRW_ID:
            sys_async_getarrw(uc);
            break;
        case SYS_VMODE_ID:
            sys_vmode(uc);
            break;
        case SYS_SET_COLOR_ID:
            sys_set_color(uc, ebx);
            break;
        case SYS_RGB_COLOR_ID:
            sys_rgb_color(ebx);
            break;
        case SYS_VCURSOR_GET_ID:
            sys_vcursor_get(uc, ebx, ecx);
            break;
        case SYS_TIME_ID:
            sys_time(uc);
            break;
        case SYS_TIME_MS_ID:
            sys_time_ms(uc);
            break;
        case SYS_YIELD_ID:
            sys_yield(uc);
            break;
        case SYS_EXIT_ID:
            sys_exit(uc, ebx);
            break;
        case SYS_SPAWN_ID:
            sys_spawn(uc, ebx, ecx, edx);
            break;
        case SYS_GET_PROC_INFO_ID:
            sys_get_proc_info(uc, ebx);
            break;
        case SYS_KILL_ID:
            sys_kill(uc, ebx);
            break;
        case SYS_GET_EXIT_CODE_ID:
            sys_get_exit_code(uc, ebx);
            break;
        case SYS_RAMINFO_ID:
            sys_raminfo(uc);
            break;
        case SYS_SET_TERM_ID:
            sys_set_term(ebx, ecx);
            break;
        case SYS_MESSAGE_SEND_ID:
            sys_message_send(uc, ebx, ecx, edx);
            break;
        case SYS_MESSAGE_RECV_ID:
            sys_message_recv(uc, ebx, ecx, edx);
            break;
        
        default:
            printf("[emu] unknown syscall %d\n", eax);
            sched_exit_current(-1);
            break;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <elf> [args...]\n", argv[0]);
        return 1;
    }
    
    vfs_init();
    vfs_mount((vfs_mount_t*) hostfs_create("root", "."));
    vfs_mount(get_ramfs("tmp"));

    enable_raw_mode();
    atexit(restore_terminal);

    sched_init();

    int pid = sched_load(argv[1], argc - 1, argv + 1);
    if (pid < 0) {
        fprintf(stderr, "Failed to load %s\n", argv[1]);
        return 1;
    }

    int exit_code = sched_run();

    restore_terminal();

    return exit_code < 0 ? 1 : 0;
}