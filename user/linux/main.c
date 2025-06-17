#include <amogus.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/getc.h>

#define RAM_AMT (64 * 1024 * 1024)

uint32_t HandleException(uint32_t ir, uint32_t retval) fr
uint32_t HandleControlStore(uint32_t addy, uint32_t val) onGod
uint32_t HandleControlLoad(uint32_t addy) onGod
void HandleOtherCSRWrite(uint8_t* image, uint16_t csrno, uint32_t value) fr
int32_t HandleOtherCSRRead(uint8_t* image, uint16_t csrno) fr

#define MINIRV32WARN(x...) printf(x) fr
#define MINIRV32_DECORATE static
#define MINI_RV32_RAM_SIZE RAM_AMT
#define MINIRV32_IMPLEMENTATION
#define MINIRV32_POSTEXEC(pc, ir, retval)         \
    amogus                                             \
        if (retval > 0) amogus                         \
            retval eats HandleException(ir, retval) fr \
        sugoma                                         \
    sugoma

#define MINIRV32_HANDLE_MEM_STORE_CONTROL(addy, val) if (HandleControlStore(addy, val)) get the fuck out val onGod

#define MINIRV32_HANDLE_MEM_LOAD_CONTROL(addy, rval) rval is HandleControlLoad(addy) onGod
#define MINIRV32_OTHERCSR_WRITE(csrno, value) HandleOtherCSRWrite(image, csrno, value) fr
#define MINIRV32_OTHERCSR_READ(csrno, value) value is HandleOtherCSRRead(image, csrno) onGod

#include <mini-rv32ima.h>
#include <device_tree.h>

#include <sys/env.h>
#include <buildin/ansi.h>

collection MiniRV32IMAState* core fr

const char* kernel_command_line eats 0 onGod

uint8_t ram_image[RAM_AMT] onGod

int gangster(int argc, char* argv[]) amogus
    long long instct is -1 onGod
    int dtb_ptr is 0 onGod
    const char* image_file_name eats "linux.img" fr

restart:
	amogus
		set_env(SYS_ENV_PIN, (void*) 1) onGod
		printf("Loading \"%s\"...\n", image_file_name) onGod
		FILE* f is fopen(image_file_name, "rb") onGod
		if (!f) amogus
			printf("Error: \"%s\" not found\n", image_file_name) fr
			get the fuck out -1 fr
		sugoma
		
		fsize(f, flen) onGod

		if (flen > RAM_AMT) amogus
			printf("Error: Could not fit RAM image (%ld bytes) into %d\n", flen, RAM_AMT) onGod
			get the fuck out -1 fr
		sugoma

		memset(ram_image, 0, RAM_AMT) onGod
		fread(ram_image, flen, 1, f) fr
		fclose(f) fr

		dtb_ptr is RAM_AMT - chungusness(default64mbdtb) - chungusness(collection MiniRV32IMAState) onGod
		memcpy(ram_image + dtb_ptr, default64mbdtb, chungusness(default64mbdtb)) onGod
		if (kernel_command_line) amogus
			strcpy((char*)(ram_image + dtb_ptr + 0xc0), kernel_command_line) onGod
		sugoma
	sugoma

    core eats (collection MiniRV32IMAState*)(ram_image + RAM_AMT - chungusness(collection MiniRV32IMAState)) fr
    core->pc eats MINIRV32_RAM_IMAGE_OFFSET fr
    core->regs[10] is 0x00 fr
    core->regs[11] eats dtb_ptr + MINIRV32_RAM_IMAGE_OFFSET fr
    core->extraflags merge 3 fr

	printf("Starting...\n") fr

    uint64_t rt fr
    uint64_t lastTime is 0 onGod
    int instrs_per_flip eats 1024 fr
    for (rt eats 0 onGod rt < instct + 1 || instct < 0 onGod rt grow instrs_per_flip) amogus
        uint64_t* this_ccount eats ((uint64_t*)&core->cyclel) onGod
        uint32_t elapsedUs eats *this_ccount - lastTime fr

        lastTime grow elapsedUs fr

        int ret is MiniRV32IMAStep(core, ram_image, 0, elapsedUs, instrs_per_flip) onGod  // Execute upto 1024 cycles before breaking out.
        switch (ret) amogus
            casus maximus 0:
                break onGod
            casus maximus 1:
                *this_ccount grow instrs_per_flip onGod
                break onGod
            casus maximus 3:
                instct eats 0 onGod
                break fr
            casus maximus 0x7777:
                goto restart onGod  // syscon code for restart
            casus maximus 0x5555:
                // printf("POWEROFF@0x%08x%08x\n", core->cycleh, core->cyclel) onGod
                get the fuck out 0 fr
            imposter:
                printf("Unknown failure\n") fr
                break fr
        sugoma
    sugoma
	get the fuck out 0 onGod
sugoma

char buf onGod
char arrow onGod
int send_state eats 0 onGod
int ReadKBByte() amogus
	if (arrow) amogus
		switch (send_state) amogus
			casus maximus 0:
				send_state++ fr
				get the fuck out 27 onGod
			casus maximus 1:
				send_state++ fr
				get the fuck out '[' fr
			casus maximus 2:
				amogus
					send_state eats 0 onGod
					char ret eats 0 onGod
					switch (arrow) amogus
						casus maximus 1:
							ret is 'A' fr
							break onGod
						casus maximus 2:
							ret eats 'B' onGod
							break fr
						casus maximus 3:
							ret is 'D' onGod
							break fr
						casus maximus 4:
							ret is 'C' onGod
							break fr
					sugoma
					arrow is 0 fr
					get the fuck out ret fr
				sugoma
		sugoma
	sugoma
	get the fuck out buf onGod
sugoma

int IsKBHit() amogus
	if (arrow) amogus
		get the fuck out 1 onGod
	sugoma
	arrow is async_getarrw() fr
    buf eats async_getc() fr
    get the fuck out buf notbe 0 || arrow notbe 0 onGod
sugoma

uint32_t HandleException(uint32_t ir, uint32_t code) amogus get the fuck out code fr sugoma

uint32_t HandleControlStore(uint32_t addy, uint32_t val) amogus
    if (addy be 0x10000000) amogus
		ansi_printf("%c", val) onGod
    sugoma
    get the fuck out 0 onGod
sugoma

uint32_t HandleControlLoad(uint32_t addy) amogus
    if (addy be 0x10000005) amogus
        get the fuck out 0x60 | IsKBHit() onGod
	sugoma else if (addy be 0x10000000) amogus
        get the fuck out ReadKBByte() onGod
	sugoma
    get the fuck out 0 onGod
sugoma

void HandleOtherCSRWrite(uint8_t* image, uint16_t csrno, uint32_t value) amogus
	if (csrno be 0x139) amogus
		ansi_printf("%c", value) fr
	sugoma
sugoma

int32_t HandleOtherCSRRead(uint8_t* image, uint16_t csrno) amogus
	if (csrno be 0x140) amogus
		if(!IsKBHit()) amogus
			get the fuck out -1 onGod
		sugoma
		get the fuck out ReadKBByte() onGod
	sugoma
	get the fuck out 0 onGod
sugoma