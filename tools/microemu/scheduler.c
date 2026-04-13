#include <emu.h>
#include <scheduler.h>
#include <fs/vfs.h>
#include <mex.h>
#include <string.h>

#define align_down(x, a) ((x) & ~((a) - 1))
#define align_up(x, a) (((x) + (a) - 1) & ~((a) - 1))

emu_proc_t procs[MAX_TASKS];
int current_pid = -1;
int next_pid = 1;

typedef struct {
    int pid;
    int code;
} exit_code_t;

exit_code_t exit_codes[MAX_EXIT_CODES];
int exit_code_idx = 0;

extern void hook_intr(uc_engine *uc, uint32_t intno, void *user_data);

void sched_init(void) {
    memset(procs, 0, sizeof(procs));
    memset(exit_codes, 0, sizeof(exit_codes));
    for (int i = 0; i < MAX_TASKS; i++) {
        procs[i].pid = -1;
    }
}

emu_proc_t *sched_get_proc(int pid) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (procs[i].pid == pid && procs[i].active) {
            return &procs[i];
        }
    }
    return NULL;
}

emu_proc_t *sched_current(void) {
    return sched_get_proc(current_pid);
}

emu_proc_t *alloc_proc(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!procs[i].active && procs[i].pid == -1) {
            return &procs[i];
        }
    }
    return NULL;
}

emu_proc_t *load_elf_into_proc(uint8_t *file, size_t elf_size) {
    Elf32_Ehdr *eh = (Elf32_Ehdr *)file;

    if (memcmp(eh->e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "[sched] not an ELF file\n");
        return NULL;
    }
    if (eh->e_machine != EM_386) {
        fprintf(stderr, "[sched] only 32-bit x86 supported\n");
        return NULL;
    }

    emu_proc_t *proc = alloc_proc();
    if (!proc) {
        fprintf(stderr, "[sched] no free process slots\n");
        exit(1);
        return NULL;
    }

    proc->pid = next_pid++;
    proc->active = true;
    proc->stopped = false;
    proc->exit_code = 0;

    uc_open(UC_ARCH_X86, UC_MODE_32, &proc->uc);

    Elf32_Phdr *ph = (Elf32_Phdr *)(file + eh->e_phoff);
    for (int i = 0; i < eh->e_phnum; i++) {
        if (ph[i].p_type != PT_LOAD) continue;

        uint32_t vaddr = ph[i].p_vaddr;
        uint32_t memsz = ph[i].p_memsz;
        uint32_t filesz = ph[i].p_filesz;
        uint32_t offset = ph[i].p_offset;

        uint32_t start = align_down(vaddr, 0x1000);
        uint32_t end = align_up(vaddr + memsz, 0x1000);
        uint32_t num_pages = (end - start) / 0x1000;

        for (uint32_t j = 0; j < num_pages; j++) {
            uc_mem_map(proc->uc, start + j * 0x1000, 0x1000, UC_PROT_ALL);
        }

        if (memsz > 0) {
            uint8_t *zeros = calloc(1, memsz);
            uc_mem_write(proc->uc, vaddr, zeros, memsz);
            free(zeros);
        }
        if (filesz > 0) {
            uc_mem_write(proc->uc, vaddr, file + offset, filesz);
        }
    }

    uc_mem_map(proc->uc, STACK_ADDR, STACK_SIZE, UC_PROT_ALL);
    uint32_t esp = STACK_ADDR + STACK_SIZE - 4;
    uc_reg_write(proc->uc, UC_X86_REG_ESP, &esp);

    uc_mem_map(proc->uc, ARGS_ADDR, ARGS_SIZE, UC_PROT_ALL);

    proc->entry = eh->e_entry;

    uc_hook_add(proc->uc, &proc->hook, UC_HOOK_INTR, hook_intr, proc, 1, 0);

    return proc;
}

void setup_proc_args(emu_proc_t *proc, const char **argv_data, const char **envp_data) {
    uint32_t str_ptr = ARGS_ADDR;
    uint32_t argv_ptrs[64];
    uint32_t envp_ptrs[64];

    int i;
    for (i = 0; argv_data[i]; i++) {
        size_t len = strlen(argv_data[i]) + 1;
        uc_mem_write(proc->uc, str_ptr, argv_data[i], len);
        argv_ptrs[i] = str_ptr;
        str_ptr += len;
    }
    argv_ptrs[i] = 0;

    for (i = 0; envp_data[i]; i++) {
        size_t len = strlen(envp_data[i]) + 1;
        uc_mem_write(proc->uc, str_ptr, envp_data[i], len);
        envp_ptrs[i] = str_ptr;
        str_ptr += len;
    }
    envp_ptrs[i] = 0;

    str_ptr = (str_ptr + 3) & ~3;

    proc->argv_addr = str_ptr;
    uc_mem_write(proc->uc, proc->argv_addr, argv_ptrs, sizeof(argv_ptrs));

    proc->envp_addr = proc->argv_addr + sizeof(argv_ptrs);
    uc_mem_write(proc->uc, proc->envp_addr, envp_ptrs, sizeof(envp_ptrs));
}

int sched_load(const char *vfs_path, int host_argc, char **host_argv) {
    file_t *f = vfs_open(vfs_path, FILE_OPEN_MODE_READ);
    if (!f) {
        fprintf(stderr, "[sched] failed to open %s\n", vfs_path);
        return -1;
    }

    uint32_t size = f->size;
    void *raw = malloc(size);
    vfs_read(f, raw, size, 0);
    vfs_close(f);

    size_t elf_size;
    uint8_t *file = mex_load(raw, size, &elf_size);
    free(raw);
    if (!file) {
        fprintf(stderr, "[sched] failed to load executable %s\n", vfs_path);
        return -1;
    }

    emu_proc_t *proc = load_elf_into_proc(file, elf_size);
    free(file);
    if (!proc) return -1;

    const char *argv_data[64];
    for (int i = 0; i < host_argc && i < 63; i++) {
        argv_data[i] = host_argv[i];
    }
    argv_data[host_argc < 63 ? host_argc : 63] = NULL;

    const char *envp_data[] = { NULL };
    setup_proc_args(proc, argv_data, envp_data);

    strncpy(proc->pwd, emu_pwd, sizeof(proc->pwd) - 1);
    proc->pwd[sizeof(proc->pwd) - 1] = '\0';

    return proc->pid;
}

int sched_spawn(uc_engine *parent_uc, uint32_t path_addr, uint32_t g_argv_addr, uint32_t g_envp_addr) {
    char path[512];
    read_emu_string(parent_uc, path_addr, path, sizeof(path));

    const char *argv_data[64];
    char argv_bufs[64][512];
    int argc = 0;
    for (int i = 0; i < 63; i++) {
        uint32_t ptr;
        uc_mem_read(parent_uc, g_argv_addr + i * 4, &ptr, 4);
        if (ptr == 0) {
            break;
        }

        read_emu_string(parent_uc, ptr, argv_bufs[i], sizeof(argv_bufs[i]));
        argv_data[i] = argv_bufs[i];
        argc++;
    }
    argv_data[argc] = NULL;

    const char *envp_data[64];
    char envp_bufs[64][512];
    int envc = 0;
    for (int i = 0; i < 63; i++) {
        uint32_t ptr;
        uc_mem_read(parent_uc, g_envp_addr + i * 4, &ptr, 4);
        if (ptr == 0) {
            break;
        }

        read_emu_string(parent_uc, ptr, envp_bufs[i], sizeof(envp_bufs[i]));
        envp_data[i] = envp_bufs[i];
        envc++;
    }
    envp_data[envc] = NULL;

    file_t *f = vfs_open(path, FILE_OPEN_MODE_READ);
    if (!f) {
        fprintf(stderr, "[sched] spawn: failed to open %s\n", path);
        return -1;
    }

    uint32_t size = f->size;
    void *raw = malloc(size);
    vfs_read(f, raw, size, 0);
    vfs_close(f);

    size_t elf_size;
    uint8_t *file = mex_load(raw, size, &elf_size);
    free(raw);
    if (!file) {
        fprintf(stderr, "[sched] spawn: failed to load %s\n", path);
        return -1;
    }

    emu_proc_t *proc = load_elf_into_proc(file, elf_size);
    free(file);
    if (!proc) {
        return -1;
    }

    setup_proc_args(proc, argv_data, envp_data);

    emu_proc_t *parent = sched_current();
    if (parent) {
        strncpy(proc->pwd, parent->pwd, sizeof(proc->pwd) - 1);
        proc->pwd[sizeof(proc->pwd) - 1] = '\0';
    } else {
        strncpy(proc->pwd, emu_pwd, sizeof(proc->pwd) - 1);
        proc->pwd[sizeof(proc->pwd) - 1] = '\0';
    }

    return proc->pid;
}

void sched_exit_current(int code) {
    emu_proc_t *proc = sched_current();
    if (!proc) {
        return;
    }

    exit_codes[exit_code_idx].pid = proc->pid;
    exit_codes[exit_code_idx].code = code;
    exit_code_idx = (exit_code_idx + 1) % MAX_EXIT_CODES;

    proc->active = false;
    proc->stopped = true;
    proc->exit_code = code;
    uc_emu_stop(proc->uc);
}

int sched_get_exit_code(int pid) {
    for (int i = 0; i < MAX_EXIT_CODES; i++) {
        if (exit_codes[i].pid == pid) {
            return exit_codes[i].code;
        }
    }
    return -1;
}

int sched_run(void) {
    int first_pid = -1;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (procs[i].active) {
            first_pid = procs[i].pid;
            break;
        }
    }

    if (first_pid == -1) return 1;

    while (1) {
        bool any_active = false;

        for (int i = 0; i < MAX_TASKS; i++) {
            if (!procs[i].active) continue;
            any_active = true;

            emu_proc_t *proc = &procs[i];
            current_pid = proc->pid;

            // argv_addr = proc->argv_addr;
            // envp_addr = proc->envp_addr;
            // strncpy(emu_pwd, proc->pwd, sizeof(emu_pwd) - 1);

            uint32_t eip;
            uc_reg_read(proc->uc, UC_X86_REG_EIP, &eip);

            if (eip == 0) {
                eip = proc->entry;
            }

            proc->stopped = false;
            uc_err err = uc_emu_start(proc->uc, eip, 0, 0, SCHED_SLICE);

            // proc->argv_addr = argv_addr;
            // proc->envp_addr = envp_addr;
            // strncpy(proc->pwd, emu_pwd, sizeof(proc->pwd) - 1);

            if (proc->stopped) {
                continue;
            }

            if (err && err != UC_ERR_OK) {
                fprintf(stderr, "[sched] pid %d emulation error: %s\n", proc->pid, uc_strerror(err));
                if (err == UC_ERR_READ_UNMAPPED) {
                    uint32_t fault_addr;
                    uc_reg_read(proc->uc, UC_X86_REG_EIP, &fault_addr);
                    fprintf(stderr, "[sched] pid %d unmapped memory access at %08x\n", proc->pid, fault_addr);
                }
                sched_exit_current(-1);
            }
        }

        if (!any_active) {
            break;
        }
    }

    for (int i = 0; i < MAX_TASKS; i++) {
        if (procs[i].uc) {
            uc_close(procs[i].uc);
            procs[i].uc = NULL;
        }
    }

    return sched_get_exit_code(first_pid);
}
