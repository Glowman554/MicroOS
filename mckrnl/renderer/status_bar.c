#include <renderer/status_bar.h>
#include <renderer/text_mode_emulation.h>
#include <stdint.h>
#include <stdio.h>
#include <memory/pmm.h>
#include <scheduler/scheduler.h>
#include <driver/clock_driver.h>
#include <utils/time.h>
#include <utils/multiboot.h>

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

int format_memory_usage(char* out_buf, uint32_t usage) {
	if (is_gb(usage)) {
		return sprintf(out_buf, "%d,%d GB", to_gb(usage), to_mb(usage) % 1024);
	} else if (is_mb(usage)) {
		return sprintf(out_buf, "%d,%d MB", to_mb(usage), to_kb(usage) % 1024);
	} else if (is_kb(usage)) {
		return sprintf(out_buf, "%d,%d KB", to_kb(usage), usage % 1024);
	} else {
		return sprintf(out_buf, "%d B", usage);
	}
}


void draw_status_bar() {
    char buffer[80] = { 0 };
    static char buffer_old[80] = { 0 };
    static bool initial = true;

    char* ptr = buffer;
    ptr += sprintf(ptr, "Used: ");
    ptr += format_memory_usage(ptr, used_memory);
    ptr += sprintf(ptr, ", Tasks: %d", get_ammount_running_tasks());
    if (global_char_output_driver) {
        ptr += sprintf(ptr, ", Terminal: %d", global_char_output_driver->current_term);
    }

    if (global_clock_driver) {
        ptr += sprintf(ptr, ", Time: ");
        time_t time = global_clock_driver->get_time(global_clock_driver);
        ptr += time_format(ptr, &time);
    }

    for (int i = 0; i < 80; i++) {
        if (buffer[i] != buffer_old[i] || initial) {
            draw_char((void*)(uint32_t)global_multiboot_info->fb_addr, 8 * i, 16 * 25, buffer[i] ? buffer[i] : ' ', 0x00000000, 0xffffffff);
            buffer_old[i] = buffer[i];
        }
    }

    initial = false;
}