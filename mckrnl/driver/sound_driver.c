#include <driver/sound_driver.h>
#include <stddef.h>

sound_driver_t* global_sound_driver = NULL;

void sound_register_if_better(struct sound_driver* d) {
	if (global_sound_driver == NULL) {
		global_sound_driver = d;
	} else {
		if (d->get_channels(d) > global_sound_driver->get_channels(global_sound_driver)) {
			global_sound_driver = d;
		}
	}
}