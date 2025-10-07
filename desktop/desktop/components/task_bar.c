#include <components/task_bar.h>

#include <fpic.h>
#include <render.h>

#include <sys/time.h>

#include <buildin/unix_time.h>
#include <string.h>
#include <stddef.h>

#include <components/window.h>

extern fpic_image_t* logo;

click_area_t logo_button;

void draw_task_bar() {

    int start_y = info.fb_height - task_bar_height;
    for (int i = 0; i < info.fb_width; i++) {
        for (int j = 0; j < task_bar_height; j++) {
            set_pixel(i, start_y + j, desktop_focus ? task_bar_color_focused : task_bar_color);
        }
    }

    draw_task_bar_infos();
    draw_fpic(logo, 0, start_y);

    logo_button.x = 0;
    logo_button.y = start_y;
    logo_button.width = 32;
    logo_button.height = 32;

    int offset = 48;
    for (int i = 0; i < max_instances; i++) {
        if (window_instances[i].window && window_instances[i].icon) {
            draw_fpic(window_instances[i].icon, offset, start_y);
            
            window_instances[i].icon_area.x = offset;
            window_instances[i].icon_area.y = start_y;
            window_instances[i].icon_area.width = 32;
            window_instances[i].icon_area.height = 32;

            offset += 40;
        }
    }
}

void draw_task_bar_infos() {
    int start_y = info.fb_height - task_bar_height;

    long long unix_time = time(NULL);
    char date[128] = { 0 };

	unix_time_to_string(unix_time, date);
    draw_string(info.fb_width - strlen(date) * 8, start_y, date, 0x0, desktop_focus ? task_bar_color_focused : task_bar_color);

    // const char* infostr = "This is a early version. USE AT YOUR OWN RISK";
    // draw_string(info.fb_width - strlen(infostr) * 8, start_y + 16, infostr, 0x0, desktop_focus ? task_bar_color_focused : task_bar_color);
}
