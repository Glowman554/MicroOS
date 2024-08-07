#include <buildin/mouse.h>
#include <buildin/graphics.h>
#define FB_SET_PX_IMPL
#include <buildin/framebuffer.h>
#include <sys/graphics.h>
#include <stdint.h>

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

uint8_t mouse_pointer[] = {
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,0,0,
	1,2,1,0,0,0,0,0,0,0,0,0,
	1,2,2,1,0,0,0,0,0,0,0,0,
	1,2,2,2,1,0,0,0,0,0,0,0,
	1,2,2,2,2,1,0,0,0,0,0,0,
	1,2,2,2,2,2,1,0,0,0,0,0,
	1,2,2,2,2,2,2,1,0,0,0,0,
	1,2,2,2,2,2,2,2,1,0,0,0,
	1,2,2,2,2,2,2,2,2,1,0,0,
	1,2,2,2,2,2,2,2,2,2,1,0,
	1,2,2,2,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,1,1,1,
	1,2,2,2,1,2,2,1,0,0,0,0,
	1,2,2,1,0,1,2,2,1,0,0,0,
	1,2,1,0,0,1,2,2,1,0,0,0,
	1,1,0,0,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0
};

extern fb_info_t fb_info;
extern uint8_t* fb;

mouse_info_t update_mouse() {
    mouse_info_t mouse;
    mouse_info(&mouse);

    if (vmode() == CUSTOM) { 
		int current_x = 0;
		int current_y = 0;
		for (int i = 0; i < mouse_pointer_len; i++) {
			if (mouse_pointer[i] == 2) {
				fb_set_pixel(&fb_info, mouse.x + current_x, mouse.y + current_y, 0xFFFFFF);
			} else if (mouse_pointer[i] == 1) {
				fb_set_pixel(&fb_info, mouse.x + current_x, mouse.y + current_y, 0x000000);
			}

			current_x++;
			if (current_x >= mouse_pointer_width) {
				current_x = 0;
				current_y++;
			}
		}
	} else {
		mouse.x /= 6;
		mouse.y /= 6;

		if (mouse.x < 80 && mouse.y < 25) {
			fb[(mouse.y * get_width() + mouse.x) * 2 + 1] = BACKGROUND_LIGHTGRAY | FOREGROUND_BLACK;
		}
	}

	return mouse;
}
