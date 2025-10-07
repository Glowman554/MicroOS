#include <window/clickarea.h>
#include <window.h>

bool check_click_area_window(click_area_t* area, mouse_info_t* info) {
    static bool trigger = false;;
#if 0
    draw_line_window(area->x, area->y, area->x + area->width, area->y + area->height, 0xffff0000);
#endif

    if (trigger && info->button_left) {
        return false;
    } else {
        trigger = false;

        if (info->button_left &&
            info->x >= area->x &&
            info->x <= area->x + area->width &&
            info->y >= area->y &&
            info->y <= area->y + area->height) {
            
            trigger = true;

            return true;
        }
    }

    return false;
}