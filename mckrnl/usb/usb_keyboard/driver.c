#include <usb_keyboard.h>

#include <string.h>
#include <memory/heap.h>
#include <stdio.h>
#include <driver/char_input_driver.h>
#include <devices/shortcut.h>

// TODO: This shouldnt be hardcoded. Also the USB keymap is incompatible with the PS/2 keymap, a better solution is needed.
const char usb_hid_to_ascii[] = {
	0, 0, 0, 0,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'\n',
	0x1B,
	'\b',
	'\t',
	' ',
	'-', '=', '[', ']', '\\',
	0,
	';', '\'', '`', ',', '.', '/',
};

const char usb_hid_to_ascii_shift[] = {
	0, 0, 0, 0,
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
	'\n',
	0x1B,
	'\b',
	'\t',
	' ',
	'_', '+', '{', '}', '|',
	0,
	':', '"', '~', '<', '>', '?',
};

#define USB_HID_KEY_RIGHT_ARROW  0x4F
#define USB_HID_KEY_LEFT_ARROW   0x50
#define USB_HID_KEY_DOWN_ARROW   0x51
#define USB_HID_KEY_UP_ARROW     0x52

#define USB_HID_KEY_F1           0x3A
#define USB_HID_KEY_F12          0x45

void usb_keyboard_update_modifiers(usb_keyboard_data_t* data, uint8_t modifiers) {
	data->special_keys_down.left_ctrl = (modifiers & USB_KB_MOD_LEFT_CTRL) != 0;
	data->special_keys_down.left_shift = (modifiers & USB_KB_MOD_LEFT_SHIFT) != 0;
	data->special_keys_down.left_alt = (modifiers & USB_KB_MOD_LEFT_ALT) != 0;
	data->special_keys_down.left_gui = (modifiers & USB_KB_MOD_LEFT_GUI) != 0;
	data->special_keys_down.right_ctrl = (modifiers & USB_KB_MOD_RIGHT_CTRL) != 0;
	data->special_keys_down.right_shift = (modifiers & USB_KB_MOD_RIGHT_SHIFT) != 0;
	data->special_keys_down.right_alt = (modifiers & USB_KB_MOD_RIGHT_ALT) != 0;
	data->special_keys_down.right_gui = (modifiers & USB_KB_MOD_RIGHT_GUI) != 0;
}

bool usb_keyboard_key_in_report(usb_keyboard_report_t* report, uint8_t key) {
	for (int i = 0; i < 6; i++) {
		if (report->keys[i] == key) {
			return true;
		}
	}
	return false;
}

char usb_keyboard_translate_key(uint8_t key, special_keys_down_t* mods) {
	bool shifted = mods->left_shift || mods->right_shift || mods->caps_lock;

	if (key < sizeof(usb_hid_to_ascii)) {
		char c;
		if (shifted) {
			c = usb_hid_to_ascii_shift[key];
		} else {
			c = usb_hid_to_ascii[key];
		}

		if (mods->caps_lock && !mods->left_shift && !mods->right_shift) {
			if (c >= 'a' && c <= 'z') {
				c = c - 'a' + 'A';
			}
		} else if (mods->caps_lock && (mods->left_shift || mods->right_shift)) {
			if (c >= 'A' && c <= 'Z') {
				c = c - 'A' + 'a';
			}
		}

		return c;
	}

	return 0;
}

void usb_keyboard_poll(usb_keyboard_data_t* data) {
	if (!data->active || data->hc == NULL || data->dev == NULL) {
		return;
	}

	memset(data->poll_buffer, 0, sizeof(data->poll_buffer));

	if (!data->hc->interrupt_transfer(data->hc, data->dev, &data->endpoint, data->poll_buffer, 8)) {
		return;
	}

	usb_keyboard_report_t* report = (usb_keyboard_report_t*)data->poll_buffer;

	bool rollover = true;
	for (int i = 0; i < 6; i++) {
		if (report->keys[i] != 0x01) {
			rollover = false;
			break;
		}
	}
	if (rollover) {
		return;
	}

	usb_keyboard_update_modifiers(data, report->modifiers);

	for (int i = 0; i < 6; i++) {
		uint8_t key = report->keys[i];
		if (key == 0) {
			continue;
		}

		if (!usb_keyboard_key_in_report(&data->prev_report, key)) {
			if (key == USB_HID_KEY_UP_ARROW) {
				data->arrow = up;
				data->special_keys_down.up_arrow = true;
			} else if (key == USB_HID_KEY_DOWN_ARROW) {
				data->arrow = down;
				data->special_keys_down.down_arrow = true;
			} else if (key == USB_HID_KEY_LEFT_ARROW) {
				data->arrow = left;
				data->special_keys_down.left_arrow = true;
			} else if (key == USB_HID_KEY_RIGHT_ARROW) {
				data->arrow = right;
				data->special_keys_down.right_arrow = true;
			} else if (key >= USB_HID_KEY_F1 && key <= USB_HID_KEY_F12) {
				global_fkey_handler(key - USB_HID_KEY_F1 + 1, false);
			} else if (key == 0x39) {
				data->special_keys_down.caps_lock = !data->special_keys_down.caps_lock;
			} else {
				char c = usb_keyboard_translate_key(key, &data->special_keys_down);

				if ((data->special_keys_down.left_ctrl || data->special_keys_down.right_ctrl) &&
				    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
					if (execute_shortcut(c)) {
						c = 0;
					}
				}

				if ((data->special_keys_down.left_ctrl || data->special_keys_down.right_ctrl) &&
				    c == 'd') {
					c = -1;
				}

				if (c != 0) {
					data->character = c;
				}
			}
		}
	}

	for (int i = 0; i < 6; i++) {
		uint8_t key = data->prev_report.keys[i];
		if (key == 0) {
			continue;
		}

		if (!usb_keyboard_key_in_report(report, key)) {
			if (key == USB_HID_KEY_UP_ARROW) {
				data->special_keys_down.up_arrow = false;
			} else if (key == USB_HID_KEY_DOWN_ARROW) {
				data->special_keys_down.down_arrow = false;
			} else if (key == USB_HID_KEY_LEFT_ARROW) {
				data->special_keys_down.left_arrow = false;
			} else if (key == USB_HID_KEY_RIGHT_ARROW) {
				data->special_keys_down.right_arrow = false;
			} else if (key >= USB_HID_KEY_F1 && key <= USB_HID_KEY_F12) {
				global_fkey_handler(key - USB_HID_KEY_F1 + 1, true);
			}
		}
	}

	memcpy(&data->prev_report, report, sizeof(usb_keyboard_report_t));
}

bool usb_keyboard_is_device_present(driver_t* driver) {
	return true;
}

char* usb_keyboard_get_device_name(driver_t* driver) {
	return "usb_keyboard";
}

void usb_keyboard_init(driver_t* driver) {
	usb_keyboard_data_t* data = (usb_keyboard_data_t*)driver->driver_specific_data;
	data->active = true;

	debugf(INFO, "usb_keyboard: initialized (addr=%d, endpoint=0x%x)", data->dev->address, data->endpoint.bEndpointAddress);

	global_char_input_driver = (char_input_driver_t*)driver;
}

char usb_keyboard_async_getc(char_input_driver_t* driver) {
	usb_keyboard_data_t* data = (usb_keyboard_data_t*)driver->driver.driver_specific_data;

	usb_keyboard_poll(data);

	char c = data->character;
	data->character = 0;

	return c;
}

char usb_keyboard_async_getarrw(char_input_driver_t* driver) {
	usb_keyboard_data_t* data = (usb_keyboard_data_t*)driver->driver.driver_specific_data;

	usb_keyboard_poll(data);

	char c = data->arrow;
	data->arrow = 0;

	return c;
}

char_input_driver_t* usb_keyboard_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* endpoint) {
	char_input_driver_t* driver = (char_input_driver_t*)kmalloc(sizeof(char_input_driver_t) + sizeof(usb_keyboard_data_t));
	memset(driver, 0, sizeof(char_input_driver_t) + sizeof(usb_keyboard_data_t));

	driver->driver.is_device_present = usb_keyboard_is_device_present;
	driver->driver.get_device_name = usb_keyboard_get_device_name;
	driver->driver.init = usb_keyboard_init;

	driver->async_getc = usb_keyboard_async_getc;
	driver->async_getarrw = usb_keyboard_async_getarrw;

	usb_keyboard_data_t* data = (usb_keyboard_data_t*)&driver[1];
	driver->driver.driver_specific_data = data;

	data->hc = hc;
	data->dev = dev;
	memcpy(&data->endpoint, endpoint, sizeof(usb_endpoint_descriptor_t));
	data->active = false;
	data->character = 0;
	data->arrow = 0;

	return driver;
}
