#include <emu.h>

#define MMAP_CHUNK_PAGES 64  // 256KB per chunk
void emu_mmap_page(uc_engine *uc, uint32_t addr) {
    uint8_t tmp;
    if (uc_mem_read(uc, addr, &tmp, 1) == UC_ERR_OK) {
        return;
    }

    uint32_t chunk_size = MMAP_CHUNK_PAGES * 0x1000;
    uint32_t chunk_start = addr & ~(chunk_size - 1);

    uc_err err = uc_mem_map(uc, chunk_start, chunk_size, UC_PROT_ALL);
    if (err == UC_ERR_OK) {
        return;
    }

    uc_mem_map(uc, addr, 0x1000, UC_PROT_ALL);
}

void sys_mmap(uc_engine *uc, uint32_t ebx) {
    emu_mmap_page(uc, ebx);
}

void sys_mmap_mapped(uc_engine *uc, uint32_t ebx) {
    uint32_t mapped = 0;
    uint8_t tmp;
    if (uc_mem_read(uc, ebx, &tmp, 1) == UC_ERR_OK) {
        mapped = 1;
    }
    uc_reg_write(uc, UC_X86_REG_ECX, &mapped);
}


