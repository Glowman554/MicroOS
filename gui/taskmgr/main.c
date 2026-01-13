#include <window.h>
#include <window/font.h>
#include <window/fpic.h>
#include <window/clickarea.h>
#include <window/strinput.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/spawn.h>
#include <sys/raminfo.h>

#define is_kb(x) ((x) >= 1024)
#define is_mb(x) ((x) >= 1024 * 1024)
#define is_gb(x) ((x) >= 1024 * 1024 * 1024)

#define to_kb(x) ((x) / 1024)
#define to_mb(x) ((x) / 1024 / 1024)
#define to_gb(x) ((x) / 1024 / 1024 / 1024)

extern fpic_image_t kill_button;

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



int main() {
    window_init(200, 400, 50, 50, "Tasks");
    window_fullscreen();

    psf1_font_t font = load_psf1_font("dev:font");

    // display: tasks (name & pid)
    // allow: kill

    task_list_t* list = malloc(sizeof(task_list_t) * 10);

    while (true) {
        window_optimize();

        if (window_redrawn()) {
            window_clear(0);
        }

        mouse_info_t info;
        window_mouse_info(&info);

        int amount = get_task_list(list, 10);
        for (int i = 0; i < amount; i++) {
            char buf[16] = { 0 };
            sprintf(buf, "%d", list[i].pid);
            draw_string_window(&font, 0, i * 16, buf, 0xffffffff, 0);
            draw_string_window(&font,5 * 8, i * 16, list[i].name, 0xffffffff, 0);

            click_area_t kill_button_area;

            kill_button_area.x = window->window_width - kill_button.width;
            kill_button_area.y = i * 16;
            kill_button_area.width = kill_button.width;
            kill_button_area.height = kill_button.height;
            draw_fpic_window(&kill_button, kill_button_area.x, kill_button_area.y);
            if (check_click_area_window(&kill_button_area, &info)) {
                kill(list[i].pid);
                // window->dirty = true;
            }
        }

        uint32_t free;
        uint32_t used;
        raminfo(&free, &used);
                
        char buffer[80] = { 0 };
        char* ptr = buffer;
        ptr += sprintf(ptr, "Memory usage: ");
        ptr += format_memory_usage(ptr, used);
        ptr += sprintf(ptr, " / ");
        ptr += format_memory_usage(ptr, free + used);

        draw_string_window(&font, 0, window->window_height - 16, buffer, 0xffffffff, 0);
    }
}