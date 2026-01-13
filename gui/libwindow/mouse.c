#include <window/mouse.h>
#include <window.h>
#include <sys/mouse.h>

#include <stdint.h>

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

extern uint8_t mouse_pointer[];

uint32_t old_pixels[mouse_pointer_len];
int old_x = 0;
int old_y = 0;

static inline void set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= window->info.fb_width || y >= window->info.fb_height || x < 0 || y < 0) {
        return;
    }
	*(uint32_t*) (window->info.fb_addr + (x * 4) + (y * window->info.fb_pitch)) = color;
}

static inline uint32_t get_pixel(uint32_t x, uint32_t y) {
    if (x >= window->info.fb_width || y >= window->info.fb_height || x < 0 || y < 0) {
        return 0;
    }
    return *(uint32_t*) (window->info.fb_addr + (x * 4) + (y * window->info.fb_pitch));
}

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

void save_new(mouse_info_t* info) {
    int current_x = 0;
	int current_y = 0;

    for (int i = 0; i < mouse_pointer_len; i++) {
        old_pixels[i] = get_pixel(info->x + current_x, info->y + current_y);

		current_x++;
		if (current_x >= mouse_pointer_width) {
			current_x = 0;
			current_y++;
		}
	}

    old_x = info->x;
    old_y = info->y;

}

void draw_mouse_pointer(mouse_info_t* mouse) {
	int current_x = 0;
	int current_y = 0;

    if (old_x != mouse->x || old_y != mouse->y) {
        restore_old();
        save_new(mouse);
    }

    for (int i = 0; i < mouse_pointer_len; i++) {
        if (mouse_pointer[i] == 2) {
            set_pixel(mouse->x + current_x, mouse->y + current_y, 0xFFFFFF);
        } else if (mouse_pointer[i] == 1) {
            set_pixel(mouse->x + current_x, mouse->y + current_y, 0x000000);
        }

        current_x++;
        if (current_x >= mouse_pointer_width) {
            current_x = 0;
            current_y++;
        }
    }
}
