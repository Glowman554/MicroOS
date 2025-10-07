#include <components/mouse.h>

#include <sys/mouse.h>

#include <render.h>

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

extern uint8_t mouse_pointer[];

mouse_info_t last_mouse;

uint32_t old_pixels[mouse_pointer_len];
int old_x = 0;
int old_y = 0;

void restore_old() {
    int current_x = 0;
	int current_y = 0;

    for (int i = 0; i < mouse_pointer_len; i++) {
        set_pixel(old_x + current_x, old_y + current_y, old_pixels[i]);

		current_x++;
		if (current_x >= mouse_pointer_width) {
			current_x = 0;
			current_y++;
		}
	}
}

void save_new(mouse_info_t info) {
    int current_x = 0;
	int current_y = 0;

    for (int i = 0; i < mouse_pointer_len; i++) {
        old_pixels[i] = get_pixel(info.x + current_x, info.y + current_y);

		current_x++;
		if (current_x >= mouse_pointer_width) {
			current_x = 0;
			current_y++;
		}
	}

    old_x = info.x;
    old_y = info.y;

}

void draw_mouse_pointer() {
	int current_x = 0;
	int current_y = 0;

    mouse_info_t mouse;
    mouse_info(&mouse);

    if (old_x != mouse.x || old_y != mouse.y) {
        restore_old();
        save_new(mouse);
    }

    for (int i = 0; i < mouse_pointer_len; i++) {
        if (mouse_pointer[i] == 2) {
            set_pixel(mouse.x + current_x, mouse.y + current_y, 0xFFFFFF);
        } else if (mouse_pointer[i] == 1) {
            set_pixel(mouse.x + current_x, mouse.y + current_y, 0x000000);
        }

        current_x++;
        if (current_x >= mouse_pointer_width) {
            current_x = 0;
            current_y++;
        }
    }

	last_mouse = mouse;
}

void mouse_save() {
	save_new(last_mouse);
}