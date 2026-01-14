#include <scheduler/loader.h>
#include <fs/vfs.h>
#include <scheduler/scheduler.h>
#include <scheduler/elf.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <utils/tinf.h>
#include <stdio.h>
#include <string.h>
#include <config.h>


const char* shebangs[] = {
	"#!",
	"//!",
	"--!",
    ";!"
};

int load_executable(int term, char* path, char** argv, char** envp) {
    file_t* file = vfs_open(path, FILE_OPEN_MODE_READ);
	if (!file) {
		debugf("Failed to open %s", path);
		return -1;
	}

	task_t* current = get_self();

	void* buffer = vmm_alloc(file->size / 4096 + 1);
	vfs_read(file, buffer, file->size, 0);

    bool is_shebang = false;
    int shebang_offset = 0;
    for (int i = 0; i < sizeof(shebangs) / sizeof(shebangs[0]); i++) {
        int len = strlen(shebangs[i]);
        if (file->size >= len) {
            if (memcmp(buffer, shebangs[i], len) == 0) {
                is_shebang = true;
                shebang_offset = len;
                break;
            }
        }
    }
	
    int pid = -1;
    if (is_shebang) {
        char executor[256] = { 0 };
        int j = 0;
        for (j = 0; j < sizeof(executor); j++) {
            char c = ((char*) buffer)[shebang_offset + j];
            if (c == '\n' || c == '\0') {
                break;
            }
            executor[j] = c;
        }
        executor[j] = '\0';

        debugf("shebang detected, executor: %s", executor);

        int argc = 0;
        while (argv[argc] != NULL) {
            argc++;
        }
        argc--; // exclude original argv[0]

        char* new_argv[argc + 3];
        new_argv[0] = executor;
        new_argv[1] = path;
        for (int i = 0; i < argc; i++) {
            new_argv[i + 2] = argv[i + 1];
        }
        new_argv[argc + 2] = NULL;

        pid = load_executable(current->term, executor, new_argv, envp);
    } else {
        pid = init_executable(current->term, buffer, argv, envp);
    }
	
    vmm_free(buffer, file->size / 4096 + 1);
	vfs_close(file);

	return pid;
}



int init_elf(int term, void* image, char** argv, char** envp) {

	struct elf_header* header = image;

	if (header->magic != ELF_MAGIC) {
		printf("ELF magic mismatch\n");
		return -1;
	}

	task_t* task = init_task(term, (void*) header->entry, false, NULL);

	struct elf_program_header* ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (int i = 0; i < header->ph_entry_count; i++, ph++) {
		void* dest = (void*) ph->virt_addr;
		void* real_dest = (void*) ALIGN_PAGE_DOWN((uintptr_t) dest);

		void* src = ((char*) image) + ph->offset;

		if (ph->type != 1) {
			continue;
		}    

		int real_size = ph->mem_size / 4096 + 1;
		if (dest != real_dest) {
			real_size++;
		}

		void* phys_loc = pmm_alloc_range(real_size);
		for (int j = 0; j < real_size; j++) {
			if (vmm_lookup((uintptr_t) real_dest + j * 4096, task->context)) {
				pmm_free(phys_loc + j * 4096);
			} else {
				vmm_map_page(task->context, (uintptr_t) real_dest + j * 4096, (uintptr_t) phys_loc + j * 4096, PTE_PRESENT | PTE_WRITE | PTE_USER);
			}
		}

		vmm_context_t old = vmm_get_current_context();
		vmm_activate_context(task->context);

		memset(dest, 0, ph->mem_size);
		memcpy(dest, src, ph->file_size);

		vmm_activate_context(&old);
	}

	int num_envp = 0;
	for (num_envp = 0; envp[num_envp] != NULL; num_envp++);

	int num_argv = 0;
	for (num_argv = 0; argv[num_argv] != NULL; num_argv++);

	debugf("copying %d arguments and %d environment variables", num_argv, num_envp);

	task->argv = (char**) vmm_alloc(1);
	vmm_map_page(task->context, (uintptr_t) task->argv + USER_SPACE_OFFSET, (uintptr_t) task->argv, PTE_PRESENT | PTE_WRITE | PTE_USER);

	for (int i = 0; i < num_argv; i++) {
		task->argv[i] = (char*) vmm_alloc(1);
		vmm_map_page(task->context, (uintptr_t) task->argv[i] + USER_SPACE_OFFSET, (uintptr_t) task->argv[i], PTE_PRESENT | PTE_WRITE | PTE_USER);
		memset(task->argv[i], 0, 0x1000);
		strcpy(task->argv[i], argv[i]);
	}

	task->argv[num_argv] = NULL;

	task->envp = (char**) vmm_alloc(1);
	vmm_map_page(task->context, (uintptr_t) task->envp + USER_SPACE_OFFSET, (uintptr_t) task->envp, PTE_PRESENT | PTE_WRITE | PTE_USER);

	for (int i = 0; i < num_envp; i++) {
		task->envp[i] = (char*) vmm_alloc(1);
		vmm_map_page(task->context, (uintptr_t) task->envp[i] + USER_SPACE_OFFSET, (uintptr_t) task->envp[i], PTE_PRESENT | PTE_WRITE | PTE_USER);
		memset(task->envp[i], 0, 0x1000);
		strcpy(task->envp[i], envp[i]);
	}

	task->envp[num_envp] = NULL;

	for (int i = 0; i < num_argv; i++) {
		task->argv[i] = (char*) ((uint32_t) task->argv[i] + USER_SPACE_OFFSET);
	}

	task->argv = (char**) ((uint32_t) task->argv + USER_SPACE_OFFSET);

	for (int i = 0; i < num_envp; i++) {
		task->envp[i] = (char*) ((uint32_t) task->envp[i] + USER_SPACE_OFFSET);
	}

	task->envp = (char**) ((uint32_t) task->envp + USER_SPACE_OFFSET);

	task->active = true;

	return task->pid;
}

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

char* mex_decompress(unsigned int decompressed_size, unsigned int compressed_size, void* content) {
	debugf("decompressing %dkb to %dkb", compressed_size / 1024, decompressed_size / 1024);

    char* dest = (char*) vmm_alloc(TO_PAGES(decompressed_size));

	unsigned int output_size = decompressed_size;
	int res = tinf_gzip_uncompress(dest, &output_size, content, compressed_size);

	if ((res != TINF_OK) || (output_size != decompressed_size)) {
		printf("decompression failed: ");
		switch (res) {
			case TINF_DATA_ERROR:
				printf("TINF_DATA_ERROR\n");
				break;
			case TINF_BUF_ERROR:
				printf("TINF_BUF_ERROR\n");
				break;
			default:
				printf("Unknown error\n");
				break;
		}
		vmm_free(dest, TO_PAGES(decompressed_size));
        abortf(true, "MEX decompression failed");
	}

    return dest;
}

int init_mex(int term, void* image, char** argv, char** envp) {
	mex_header_t* header = image;
	void* content = (void*) header + sizeof(mex_header_t);

	if (memcmp(header->header, "MEX", 4) != 0) {
		printf("MEX magic mismatch\n");
		return -1;
	}

    debugf("Loading MEX executable, author: %s", header->programAuthor);

	unsigned int decompressed_size = read_le32(content + header->elfSizeCompressed - 4);
    char* dest = mex_decompress(decompressed_size, header->elfSizeCompressed, content);

	int pid = init_elf(term, dest, argv, envp);

	vmm_free(dest, TO_PAGES(decompressed_size));
	return pid;
}

int init_mex_v2(int term, void* image, char** argv, char** envp) {
    mex_header_v2_t* header = image;
    void* content = (void*) header + sizeof(mex_header_v2_t);

    if (memcmp(header->header, "M2X", 4) != 0) {
        printf("MEX v2 magic mismatch\n");
        return -1;
    }

    debugf("Loading MEX v2 executable, author: %s, flags: 0x%x, abiVersion: %d", header->programAuthor, header->flags, header->abiVersion);

    if (header->abiVersion != ABI_VERSION) {
        printf("WARNING: ABI version mismatch: expected %d, got %d\n", ABI_VERSION, header->abiVersion);
        printf("WARNING: This may lead to unexpected behavior or crashes!\n");
    }

    if ((header->flags & M_COMPRESSED_FLAG) == 0) {
        // not compressed
        return init_elf(term, content, argv, envp);
    }

    unsigned int decompressed_size = read_le32(content + header->elfSizeCompressed - 4);
    debugf("decompressing %dkb to %dkb", header->elfSizeCompressed / 1024, decompressed_size / 1024);

    char* dest = mex_decompress(decompressed_size, header->elfSizeCompressed, content);
    if (dest == NULL) {
        return -1;
    }

    int pid = init_elf(term, dest, argv, envp);
    vmm_free(dest, TO_PAGES(decompressed_size));
    return pid;
}

int init_executable(int term, void* image, char** argv, char** envp) {
	debugf("Loading executable at %p", image);
	
	for (int i = 0; argv[i] != NULL; i++) {
		debugf("argv[%d]: %s", i, argv[i]);
	}

	for (int i = 0; envp[i] != NULL; i++) {
		debugf("envp[%d]: %s", i, envp[i]);
	}

	mex_header_t* header = image;
	if (memcmp(header->header, "MEX", 4) == 0) {
		return init_mex(term, image, argv, envp);
	}
	
	mex_header_v2_t* header_v2 = image;
	if (memcmp(header_v2->header, "M2X", 4) == 0) {
		return init_mex_v2(term, image, argv, envp);
	}

	return init_elf(term, image, argv, envp);
}