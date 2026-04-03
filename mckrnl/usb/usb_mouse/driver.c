#include <usb_mouse.h>

#include <string.h>
#include <memory/heap.h>
#include <stdio.h>

#define USB_MOUSE_POLL_LEN 8

void usb_mouse_poll(usb_mouse_data_t* data) {
	if (!data->active || data->hc == NULL || data->dev == NULL) {
		return;
	}

	memset(data->poll_buffer, 0, sizeof(data->poll_buffer));

	uint16_t len = data->endpoint.wMaxPacketSize;
	if (len == 0 || len > USB_MOUSE_POLL_LEN) {
		len = USB_MOUSE_POLL_LEN;
	}

	if (!data->hc->interrupt_transfer(data->hc, data->dev, &data->endpoint, data->poll_buffer, len)) {
		return;  
	}

	uint8_t buttons = data->poll_buffer[0];
	int8_t dx = (int8_t)data->poll_buffer[1];
	int8_t dy = (int8_t)data->poll_buffer[2];

	bool left_button = (buttons & 0x01) != 0;
	bool right_button = (buttons & 0x02) != 0;
	bool middle_button = (buttons & 0x04) != 0;

	int x = data->mouse->info.x + (int)dx;
	int y = data->mouse->info.y + (int)dy;

	if (x < 0) {
		x = 0;
	}
	if (y < 0) {
		y = 0;
	}

	mouse_update(data->mouse, x, y, left_button, right_button, middle_button);
}

bool usb_mouse_is_device_present(driver_t* driver) {
	return true;
}

char* usb_mouse_get_device_name(driver_t* driver) {
	return "usb_mouse";
}

void usb_mouse_poll_callback(mouse_driver_t* driver) {
	usb_mouse_data_t* data = (usb_mouse_data_t*)driver->driver.driver_specific_data;
	usb_mouse_poll(data);
}

void usb_mouse_init(driver_t* driver) {
	usb_mouse_data_t* data = (usb_mouse_data_t*)driver->driver_specific_data;
	data->active = true;

	debugf(INFO, "usb_mouse: initialized (addr=%d, endpoint=0x%x)", data->dev->address, data->endpoint.bEndpointAddress);

	global_mouse_driver = data->mouse;
	debugf(INFO, "usb_mouse: set as global mouse driver");
}

mouse_driver_t* usb_mouse_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* endpoint) {
	mouse_driver_t* driver = (mouse_driver_t*)kmalloc(sizeof(mouse_driver_t) + sizeof(usb_mouse_data_t));
	memset(driver, 0, sizeof(mouse_driver_t) + sizeof(usb_mouse_data_t));

	driver->driver.is_device_present = usb_mouse_is_device_present;
	driver->driver.get_device_name = usb_mouse_get_device_name;
	driver->driver.init = usb_mouse_init;
	driver->poll = usb_mouse_poll_callback;

	usb_mouse_data_t* data = (usb_mouse_data_t*)&driver[1];
	driver->driver.driver_specific_data = data;

	data->hc = hc;
	data->dev = dev;
	data->mouse = driver;
	memcpy(&data->endpoint, endpoint, sizeof(usb_endpoint_descriptor_t));
	data->active = false;

	return driver;
}
