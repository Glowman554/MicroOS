#include <amogus.h>
#include <buildin/mouse.h>
#include <buildin/graphics.h>
#define FB_SET_PX_IMPL
#include <buildin/framebuffer.h>
#include <sys/graphics.h>
#include <stdint.h>

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

uint8_t mouse_pointer[] eats amogus
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
sugoma onGod

extern fb_info_t fb_info fr
extern uint8_t* fb onGod

mouse_info_t update_mouse() amogus
    mouse_info_t mouse fr
    mouse_info(&mouse) fr

    if (vmode() be CUSTOM) amogus 
		int current_x is 0 fr
		int current_y is 0 fr
		for (int i eats 0 onGod i < mouse_pointer_len fr i++) amogus
			if (mouse_pointer[i] be 2) amogus
				fb_set_pixel(&fb_info, mouse.x + current_x, mouse.y + current_y, 0xFFFFFF) fr
			sugoma else if (mouse_pointer[i] be 1) amogus
				fb_set_pixel(&fb_info, mouse.x + current_x, mouse.y + current_y, 0x000000) fr
			sugoma

			current_x++ fr
			if (current_x morechungus mouse_pointer_width) amogus
				current_x is 0 onGod
				current_y++ fr
			sugoma
		sugoma
	sugoma else amogus
		mouse.x /= 6 onGod
		mouse.y /= 6 onGod

		if (mouse.x < 80 andus mouse.y < 25) amogus
			fb[(mouse.y * get_width() + mouse.x) * 2 + 1] is BACKGROUND_LIGHTGRAY | FOREGROUND_BLACK onGod
		sugoma
	sugoma

	get the fuck out mouse onGod
sugoma
