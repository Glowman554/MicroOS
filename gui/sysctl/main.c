#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/clickarea.h>
#include <stdio.h>
#include <sys/time.h>
#include <buildin/unix_time.h>
#include <sys/raminfo.h>
#include <sys/env.h>

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

extern fpic_image_t reboot;
extern fpic_image_t shutdown;

void format_memory_ussage(char* out_buf, uint32_t ussage) {
	if (is_gb(ussage)) {
		sprintf(out_buf, "%d,%d GB", to_gb(ussage), to_mb(ussage) % 1024);
	} else if (is_mb(ussage)) {
		sprintf(out_buf, "%d,%d MB", to_mb(ussage), to_kb(ussage) % 1024);
	} else if (is_kb(ussage)) {
		sprintf(out_buf, "%d,%d KB", to_kb(ussage), ussage % 1024);
	} else {
		sprintf(out_buf, "%d B", ussage);
	}
}

int main() {
    window_init(200, 400, 50, 50, "System control");
    window_fullscreen();

    psf1_font_t font = load_psf1_font("dev:font");

    // display: time, timems, raminfo
    // allow: reboot, shutdown

    click_area_t reboot_area;
    click_area_t shutdowm_area;

    while (true) {
        window_optimize();

        if (window_redrawn()) {
            window_clear(0);
            
            draw_fpic_window(&reboot, window->window_width - 32, 0);
            reboot_area.x = window->window_width - 32;
            reboot_area.y = 0;
            reboot_area.width = 32;
            reboot_area.height = 32;

            draw_fpic_window(&shutdown, window->window_width - 32, 32);
            shutdowm_area.x = window->window_width - 32;
            shutdowm_area.y = 32;
            shutdowm_area.width = 32;
            shutdowm_area.height = 32;
            
        }

        char timebuf[128] = { 0 };
        unix_time_to_string(time(NULL), timebuf);

        char timemsbuf[32] = { 0 };
        sprintf(timemsbuf, "%d ms", time_ms());

        uint32_t free;
        uint32_t used;
        raminfo(&free, &used);

        char total_memory_str[32] = { 0 };
        char free_memory_str[32] = { 0 };
        char used_memory_str[32] = { 0 };

        format_memory_ussage(total_memory_str, free + used);
        format_memory_ussage(free_memory_str, free);
        format_memory_ussage(used_memory_str, used);


        int line = 0;
        draw_string_window(&font, 0, line * 16, "TIME:", 0xffffffff, 0x0);
        draw_string_window(&font, 10 * 8, line * 16, timebuf, 0xffffffff, 0x0);
        line++;
        
        draw_string_window(&font, 0, line * 16, "TIMEMS:", 0xffffffff, 0x0);
        draw_string_window(&font, 10 * 8, line * 16, timemsbuf, 0xffffffff, 0x0);
        line++;

        line++;

        draw_string_window(&font, 0, line * 16, "TOTAL:", 0xffffffff, 0x0);
        draw_string_window(&font, 10 * 8, line * 16, total_memory_str, 0xffffffff, 0x0);
        line++;

        draw_string_window(&font, 0, line * 16, "FREE:", 0xffffffff, 0x0);
        draw_string_window(&font, 10 * 8, line * 16, free_memory_str, 0xffffffff, 0x0);
        line++;

        draw_string_window(&font, 0, line * 16, "USED:", 0xffffffff, 0x0);
        draw_string_window(&font, 10 * 8, line * 16, used_memory_str, 0xffffffff, 0x0);
        line++;

        mouse_info_t info;
        window_mouse_info(&info);

        if (check_click_area_window(&reboot_area, &info)) {
    		env(SYS_PWR_RESET_ID);
        }
        if (check_click_area_window(&shutdowm_area, &info)) {
    	    env(SYS_PWR_SHUTDOWN_ID);
        }
    }
}