#pragma once

#include <driver/usb.h>
#include <driver/mouse_driver.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct usb_mouse_data {
	usb_hc_driver_t* hc;
	usb_device_t* dev;
	usb_endpoint_descriptor_t endpoint;

	mouse_driver_t* mouse;

	uint8_t poll_buffer[8];
	bool active;
} usb_mouse_data_t;

mouse_driver_t* usb_mouse_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* endpoint);

void usb_mouse_poll(usb_mouse_data_t* data);
