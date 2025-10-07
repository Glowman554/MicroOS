#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/getc.h>


#include <window.h>
#include <window/vconsole.h>

#define RAM_AMT (64 * 1024 * 1024)

uint32_t HandleException(uint32_t ir, uint32_t retval);
uint32_t HandleControlStore(uint32_t addy, uint32_t val);
uint32_t HandleControlLoad(uint32_t addy);
void HandleOtherCSRWrite(uint8_t* image, uint16_t csrno, uint32_t value);
int32_t HandleOtherCSRRead(uint8_t* image, uint16_t csrno);

#define MINIRV32WARN(x...) printf(x);
#define MINIRV32_DECORATE static
#define MINI_RV32_RAM_SIZE RAM_AMT
#define MINIRV32_IMPLEMENTATION
#define MINIRV32_POSTEXEC(pc, ir, retval)         \
    {                                             \
        if (retval > 0) {                         \
            retval = HandleException(ir, retval); \
        }                                         \
    }

#define MINIRV32_HANDLE_MEM_STORE_CONTROL(addy, val) if (HandleControlStore(addy, val)) return val;

#define MINIRV32_HANDLE_MEM_LOAD_CONTROL(addy, rval) rval = HandleControlLoad(addy);
#define MINIRV32_OTHERCSR_WRITE(csrno, value) HandleOtherCSRWrite(image, csrno, value);
#define MINIRV32_OTHERCSR_READ(csrno, value) value = HandleOtherCSRRead(image, csrno);

#include <mini-rv32ima.h>
#include <device_tree.h>

#include <sys/env.h>
#include <buildin/ansi.h>

struct MiniRV32IMAState* core;

const char* kernel_command_line = 0;

uint8_t ram_image[RAM_AMT];

int main(int argc, char* argv[]) {
	window_init(16 * 25, 8 * 80, 50, 50, "Linux");

    psf1_font_t font = load_psf1_font("dev:font");
    vconsole_init(&font);

	if (argc != 2) {
		window_async_getc();
		while (window_async_getc() != ' ') {
			window_optimize();

			if (window_redrawn()) {
				window_clear(0);
				draw_string_window(&font, 0, 0, "Please use the explorer to open an image file", 0xffffffff, 0);
				draw_string_window(&font, 0, 16, "Or press <space> to use the default one", 0xffffffff, 0);
			}
		}
		window_clear(0);
	}

    long long instct = -1;
    int dtb_ptr = 0;
    const char* image_file_name = argc == 2 ? argv[1] : "linux.img";

restart:
	{
		vconsole_puts("Loading...\n");
		FILE* f = fopen(image_file_name, "rb");
		if (!f) {
			printf("Error: \"%s\" not found\n", image_file_name);
			return -1;
		}
		
		fsize(f, flen);

		if (flen > RAM_AMT) {
			printf("Error: Could not fit RAM image (%ld bytes) into %d\n", flen, RAM_AMT);
			return -1;
		}

		memset(ram_image, 0, RAM_AMT);
		fread(ram_image, flen, 1, f);
		fclose(f);

		dtb_ptr = RAM_AMT - sizeof(default64mbdtb) - sizeof(struct MiniRV32IMAState);
		memcpy(ram_image + dtb_ptr, default64mbdtb, sizeof(default64mbdtb));
		if (kernel_command_line) {
			strcpy((char*)(ram_image + dtb_ptr + 0xc0), kernel_command_line);
		}
	}

    core = (struct MiniRV32IMAState*)(ram_image + RAM_AMT - sizeof(struct MiniRV32IMAState));
    core->pc = MINIRV32_RAM_IMAGE_OFFSET;
    core->regs[10] = 0x00;
    core->regs[11] = dtb_ptr + MINIRV32_RAM_IMAGE_OFFSET;
    core->extraflags |= 3;

	vconsole_puts("Starting...\n");

    uint64_t rt;
    uint64_t lastTime = 0;
    int instrs_per_flip = 1024;
    for (rt = 0; rt < instct + 1 || instct < 0; rt += instrs_per_flip) {
        uint64_t* this_ccount = ((uint64_t*)&core->cyclel);
        uint32_t elapsedUs = *this_ccount - lastTime;

        lastTime += elapsedUs;

        int ret = MiniRV32IMAStep(core, ram_image, 0, elapsedUs, instrs_per_flip);  // Execute upto 1024 cycles before breaking out.
        switch (ret) {
            case 0:
                break;
            case 1:
                *this_ccount += instrs_per_flip;
                break;
            case 3:
                instct = 0;
                break;
            case 0x7777:
                goto restart;  // syscon code for restart
            case 0x5555:
                // printf("POWEROFF@0x%08x%08x\n", core->cycleh, core->cyclel);
                return 0;
            default:
                printf("Unknown failure\n");
                break;
        }
    }
	return 0;
}

char buf;
char arrow;
int send_state = 0;
int ReadKBByte() {
	if (arrow) {
		switch (send_state) {
			case 0:
				send_state++;
				return 27;
			case 1:
				send_state++;
				return '[';
			case 2:
				{
					send_state = 0;
					char ret = 0;
					switch (arrow) {
						case 1:
							ret = 'A';
							break;
						case 2:
							ret = 'B';
							break;
						case 3:
							ret = 'D';
							break;
						case 4:
							ret = 'C';
							break;
					}
					arrow = 0;
					return ret;
				}
		}
	}
	return buf;
}

int IsKBHit() {
	if (arrow) {
		return 1;
	}
	arrow = window_async_getarrw();
    buf = window_async_getc();
    return buf != 0 || arrow != 0;
}

uint32_t HandleException(uint32_t ir, uint32_t code) { return code; }

uint32_t HandleControlStore(uint32_t addy, uint32_t val) {
    if (addy == 0x10000000) {
		vconsole_putc(val);
    }
    return 0;
}

uint32_t HandleControlLoad(uint32_t addy) {
    if (addy == 0x10000005) {
        return 0x60 | IsKBHit();
	} else if (addy == 0x10000000) {
        return ReadKBByte();
	}
    return 0;
}

void HandleOtherCSRWrite(uint8_t* image, uint16_t csrno, uint32_t value) {
	if (csrno == 0x139) {
		vconsole_putc(value);
	}
}

int32_t HandleOtherCSRRead(uint8_t* image, uint16_t csrno) {
	if (csrno == 0x140) {
		if(!IsKBHit()) {
			return -1;
		}
		return ReadKBByte();
	}
	return 0;
}