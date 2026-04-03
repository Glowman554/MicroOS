#pragma once

#include <driver/usb.h>
#include <driver/char_input_driver.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct usb_keyboard_data {
	usb_hc_driver_t* hc;
	usb_device_t* dev;
	usb_endpoint_descriptor_t endpoint;

	usb_keyboard_report_t current_report;
	usb_keyboard_report_t prev_report;

	special_keys_down_t special_keys_down;
	char character;
	char arrow;

	uint8_t poll_buffer[8];
	bool active;
} usb_keyboard_data_t;

char_input_driver_t* usb_keyboard_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* endpoint);

void usb_keyboard_poll(usb_keyboard_data_t* data);
