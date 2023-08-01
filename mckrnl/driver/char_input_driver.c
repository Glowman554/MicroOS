#include <driver/char_input_driver.h>

#include <fs/vfs.h>
#include <stdio.h>
#include <utils/string.h>


bool default_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* default_keyboard_get_device_name(driver_t* driver) {
	return "default_keyboard";
}

void default_keyboard_init(driver_t* driver) {}



char default_keyboard_async_getc(char_input_driver_t* driver) {
	return 0;
}

char default_keyboard_async_getarrw(char_input_driver_t* driver) {
	return 0;
}

char_input_driver_t default_char_input = {
    .driver = {
        .is_device_present = default_keyboard_is_device_present,
        .get_device_name = default_keyboard_get_device_name,
        .init = default_keyboard_init
    },
    .async_getc = default_keyboard_async_getc,
    .async_getarrw = default_keyboard_async_getarrw
};

char_input_driver_t* global_char_input_driver = &default_char_input;

keymap_t cached_keymap;
char cached_keymap_path[128] = { 0 };

char keymap(char* keymap_path, uint8_t key, special_keys_down_t* special_keys_down) {
	if (strcmp(keymap_path, cached_keymap_path) != 0) {

		file_t* file = vfs_open(keymap_path, FILE_OPEN_MODE_READ);
		if (file == NULL) {
			debugf("input::keymap: keymap %s not found", keymap_path);
			return 0;
		}

		keymap_t keymap_;
		
		vfs_read(file, &keymap_, sizeof(keymap_t), 0);
		vfs_close(file);

		cached_keymap = keymap_;
		strcpy(cached_keymap_path, keymap_path);
	}

	bool shift = special_keys_down->left_shift || special_keys_down->right_shift;
	bool alt = special_keys_down->left_alt || special_keys_down->right_alt;

	if (special_keys_down->caps_lock) {
		if (shift) {
			return cached_keymap.layout_normal[key];
		} else {
			return cached_keymap.layout_shift[key];
		}
	} else if (shift) {
		return cached_keymap.layout_shift[key];
	} else if (alt) {
		return cached_keymap.layout_alt[key];
	} else {
		return cached_keymap.layout_normal[key];
	}
}