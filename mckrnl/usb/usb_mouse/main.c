#include <usb_mouse.h>
#include <module.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

void usb_mouse_attach(usb_device_driver_t* drv, usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep) {
	debugf(INFO, "usb_mouse: attaching to device addr=%d", dev->address);

	if (ep == NULL) {
		debugf(WARNING, "usb_mouse: no endpoint descriptor, cannot attach");
		return;
	}

	debugf(INFO, "usb_mouse: endpoint addr=0x%x attr=0x%x maxpkt=%d interval=%d", ep->bEndpointAddress, ep->bmAttributes, ep->wMaxPacketSize, ep->bInterval);

	usb_setup_packet_t setup;
	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_INTERFACE;
	setup.bRequest = USB_HID_SET_PROTOCOL;
	setup.wValue = 0;  // Boot protocol
	setup.wIndex = iface->bInterfaceNumber;
	setup.wLength = 0;
	if (!hc->control_transfer(hc, dev, &setup, NULL, 0)) {
		debugf(WARNING, "usb_mouse: SET_PROTOCOL(boot) failed, continuing anyway");
	}

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_INTERFACE;
	setup.bRequest = USB_HID_SET_IDLE;
	setup.wValue = 0;
	setup.wIndex = iface->bInterfaceNumber;
	setup.wLength = 0;
	hc->control_transfer(hc, dev, &setup, NULL, 0);

	mouse_driver_t* mouse = usb_mouse_create(hc, dev, ep);
	register_driver((driver_t*)mouse);
}

usb_device_driver_t usb_mouse_device_driver = {
	.name = "usb_mouse",
	.interface_class = USB_CLASS_HID,
	.interface_subclass = USB_HID_SUBCLASS_BOOT,
	.interface_protocol = USB_HID_PROTOCOL_MOUSE,
	.attach = usb_mouse_attach,
};

void main() {
}

void stage_driver() {
	register_usb_device_driver(&usb_mouse_device_driver);
}

define_module("usb_mouse", main, stage_driver, NULL);
