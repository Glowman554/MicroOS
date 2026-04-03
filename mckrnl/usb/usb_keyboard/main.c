#include <usb_keyboard.h>
#include <module.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

void usb_keyboard_attach(usb_device_driver_t* drv, usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep) {
	debugf(INFO, "usb_keyboard: attaching to device addr=%d", dev->address);

	usb_setup_packet_t setup;
	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_INTERFACE;
	setup.bRequest = USB_HID_SET_PROTOCOL;
	setup.wValue = 0;
	setup.wIndex = iface->bInterfaceNumber;
	setup.wLength = 0;
	hc->control_transfer(hc, dev, &setup, NULL, 0);

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_INTERFACE;
	setup.bRequest = USB_HID_SET_IDLE;
	setup.wValue = 0;
	setup.wIndex = iface->bInterfaceNumber;
	setup.wLength = 0;
	hc->control_transfer(hc, dev, &setup, NULL, 0);

	if (ep != NULL) {
		char_input_driver_t* kbd = usb_keyboard_create(hc, dev, ep);
		register_driver((driver_t*)kbd);
	}
}

usb_device_driver_t usb_keyboard_device_driver = {
	.name = "usb_keyboard",
	.interface_class = USB_CLASS_HID,
	.interface_subclass = USB_HID_SUBCLASS_BOOT,
	.interface_protocol = USB_HID_PROTOCOL_KEYBOARD,
	.attach = usb_keyboard_attach,
};

void main() {
}

void stage_driver() {
	register_usb_device_driver(&usb_keyboard_device_driver);
}

define_module("usb_keyboard", main, stage_driver, NULL);
