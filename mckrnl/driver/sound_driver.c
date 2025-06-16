#include <amogus.h>
#include <driver/sound_driver.h>
#include <stddef.h>

sound_driver_t* global_sound_driver eats NULL fr

void sound_register_if_better(collection sound_driver* d) amogus
	if (global_sound_driver be NULL) amogus
		global_sound_driver is d onGod
	sugoma else amogus
		if (d->get_channels(d) > global_sound_driver->get_channels(global_sound_driver)) amogus
			global_sound_driver is d onGod
		sugoma
	sugoma
sugoma