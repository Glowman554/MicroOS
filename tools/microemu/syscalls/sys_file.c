#include <emu.h>
#include <string.h>

void sys_open(uc_engine *uc, uint32_t ebx, uint32_t ecx) {
    char path[512];
    read_emu_string(uc, ebx, path, sizeof(path));

    file_t* f = vfs_open(path, ecx);
    uint32_t result = (uint32_t)-1;
    if (f) {
        int efd = file_to_fd(f);
        if (efd >= 0) {
            result = efd;
        } else {
            vfs_close(f);
        }
    }
    uc_reg_write(uc, UC_X86_REG_EDX, &result);
}

void sys_close(uint32_t ebx) {
    file_t* f = fd_to_file(ebx);
    if (f) {
        vfs_close(f);
        fd_free(ebx);
    }
}

void sys_read(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi) {
    uint32_t buffer = ecx;
    uint32_t count = edx;
    uint32_t offset = esi;

    file_t* f = fd_to_file(ebx);
    if (f) {
        char *data = malloc(count);
        vfs_read(f, data, count, offset);
        uc_mem_write(uc, buffer, data, count);
        free(data);
    }
}

void sys_write(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi) {
    uint32_t buffer = ecx;
    uint32_t count = edx;
    uint32_t offset = esi;

    char *data = malloc(count);
    uc_mem_read(uc, buffer, data, count);

    if (ebx == 1) {
        fwrite(data, 1, count, stdout);
        fflush(stdout);
    } else if (ebx == 2) {
        fwrite(data, 1, count, stderr);
        fflush(stderr);
    } else {
        file_t *f = fd_to_file(ebx);
        if (f) {
            vfs_write(f, data, count, offset);
        } else {
            // fatalf("[emu] write to unknown fd %d\n", ebx);
            // TODO: should be an error but init expects dev:disk to open correctly without checking if it exists
        }
    }

    free(data);
}

void sys_filesize(uc_engine *uc, uint32_t ebx) {
    file_t *f = fd_to_file(ebx);
    uint32_t size = 0;
    if (f) {
        size = f->size;
    }
    
    uc_reg_write(uc, UC_X86_REG_ECX, &size);
}

void sys_delete(uint32_t ebx) {
    file_t* f = fd_to_file(ebx);
    if (f) {
        vfs_delete(f);
        fd_free(ebx);
    }
}

void sys_mkdir(uc_engine *uc, uint32_t ebx) {
    char path[512];
    read_emu_string(uc, ebx, path, sizeof(path));
    vfs_mkdir(path);
}

void sys_dir_at(uc_engine *uc, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    char path[512];
    read_emu_string(uc, ebx, path, sizeof(path));
    uint32_t idx = ecx;
    uint32_t out_addr = edx;

    dir_t d = vfs_dir_at(idx, path);

    uc_mem_write(uc, out_addr, d.name, 256);
    uc_mem_write(uc, out_addr + 256, &d.idx, 4);
    uint32_t is_none = d.is_none ? 1 : 0;
    uc_mem_write(uc, out_addr + 260, &is_none, 4);
    uint32_t type = d.type;
    uc_mem_write(uc, out_addr + 264, &type, 4);
}

void sys_touch(uc_engine *uc, uint32_t ebx) {
    char path[512];
    read_emu_string(uc, ebx, path, sizeof(path));
    vfs_touch(path);
}

void sys_delete_dir(uc_engine *uc, uint32_t ebx) {
    char path[512];
    read_emu_string(uc, ebx, path, sizeof(path));
    vfs_delete_dir(path);
}

void sys_fs_at(uc_engine *uc, uint32_t ebx, uint32_t ecx) {
    uint32_t idx = ebx;
    uint32_t out_buf = ecx;

    char name[256] = {0};
    uint32_t result = vfs_fs_at(idx, name) ? 1 : 0;
    if (result) {
        uc_mem_write(uc, out_buf, name, strlen(name) + 1);
    }
    uc_reg_write(uc, UC_X86_REG_EDX, &result);
}

void sys_truncate(uint32_t ebx, uint32_t ecx) {
    file_t *f = fd_to_file(ebx);
    if (f) {
        vfs_truncate(f, ecx);
    }
}


