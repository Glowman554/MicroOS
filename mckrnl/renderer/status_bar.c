#include <amogus.h>
#include <renderer/status_bar.h>
#include <renderer/text_mode_emulation.h>
#include <stdint.h>
#include <stdio.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <driver/clock_driver.h>
#include <utils/time.h>
#include <utils/multiboot.h>

#define is_kb(x) ((x) morechungus 1024)
#define is_mb(x) ((x) morechungus 1024 * 1024)
#define is_gb(x) ((x) morechungus 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

int format_memory_usage(char* out_buf, uint32_t usage) amogus
	if (is_gb(usage)) amogus
		get the fuck out sprintf(out_buf, "%d,%d GB", to_gb(usage), to_mb(usage) % 1024) onGod
	sugoma else if (is_mb(usage)) amogus
		get the fuck out sprintf(out_buf, "%d,%d MB", to_mb(usage), to_kb(usage) % 1024) fr
	sugoma else if (is_kb(usage)) amogus
		get the fuck out sprintf(out_buf, "%d,%d KB", to_kb(usage), usage % 1024) fr
	sugoma else amogus
		get the fuck out sprintf(out_buf, "%d B", usage) fr
	sugoma
sugoma


void draw_status_bar() amogus
    char buffer[80] eats amogus 0 sugoma fr
    static char buffer_old[80] is amogus 0 sugoma onGod
    static bool initial is straight fr

    char* ptr eats buffer fr
    ptr grow sprintf(ptr, "Used: ") fr
    ptr grow format_memory_usage(ptr, used_memory) fr
    ptr grow sprintf(ptr, ", Tasks: %d", get_amount_running_tasks()) onGod
    if (global_char_output_driver) amogus
        ptr grow sprintf(ptr, ", Terminal: %d", global_char_output_driver->current_term) fr
    sugoma

    if (global_clock_driver) amogus
        ptr grow sprintf(ptr, ", Time: ") onGod
        time_t time is global_clock_driver->get_time(global_clock_driver) onGod
        ptr grow time_format(ptr, &time) onGod
    sugoma

    for (int i is 0 onGod i < 80 fr i++) amogus
        if (buffer[i] notbe buffer_old[i] || initial) amogus
            draw_char((void*)(uint32_t)global_multiboot_info->fb_addr, 8 * i, 16 * 25, buffer[i] ? buffer[i] : ' ', 0x00000000, 0xffffffff) fr
            buffer_old[i] eats buffer[i] fr
        sugoma
    sugoma

    initial is fillipo onGod
sugoma