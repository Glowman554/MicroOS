#include <components/window.h>
#include <render.h>
#include <fpic.h>

extern fpic_image_t* exit_button;
extern fpic_image_t* minimize_button;

window_instance_t window_instances[max_instances] = { 0 };


#define border_color 0xffffffff
#define border_color_focus 0xff11ffff

void draw_window(window_instance_t* instance) {
    window_t* window = instance->window;
    
    uint32_t this_border_color = border_color;
    if (window->focus) {
        this_border_color = border_color_focus;
    }

    for (int i = -1; i < window->window_width + 1; i++) {
        set_pixel(window->window_x + i, window->window_y + window->window_height, this_border_color);
        for (int j = 0; j < 18; j++) {
            set_pixel(window->window_x + i, window->window_y - 1 - j, this_border_color);
        }
    }

    for (int j = 0; j < window->window_height; j++) {
        set_pixel(window->window_x - 1, window->window_y + j, border_color);
        set_pixel(window->window_x + window->window_width, window->window_y + j, this_border_color);
    }

    draw_string(window->window_x, window->window_y - 17, window->name, 0, this_border_color);

    instance->exit_button.x = window->window_x + window->window_width - 34 * 1;
    instance->exit_button.y = window->window_y - 17;
    instance->exit_button.width = 33;
    instance->exit_button.height = 16;

    instance->minimize_button.x = window->window_x + window->window_width - 34 * 2;
    instance->minimize_button.y = window->window_y - 17;
    instance->minimize_button.width = 33;
    instance->minimize_button.height = 16;

    instance->drag_area.x = window->window_x;
    instance->drag_area.y = window->window_y - 18;
    instance->drag_area.width = window->window_width - 34 * 2;
    instance->drag_area.height = 16;

    instance->resize_area.x = window->window_x + window->window_width;
    instance->resize_area.y = window->window_y + window->window_height;
    instance->resize_area.width = 6;
    instance->resize_area.height = 6;

    draw_fpic(exit_button, instance->exit_button.x, instance->exit_button.y);
    draw_fpic(minimize_button, instance->minimize_button.x, instance->minimize_button.y);

    window->dirty = false;
    window->redrawn = true;
}
