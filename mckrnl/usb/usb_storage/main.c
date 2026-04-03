#include <usb_storage.h>
#include <module.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <memory/heap.h>

void usb_storage_attach(usb_device_driver_t* drv, usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep) {
	debugf(INFO, "usb_storage: attaching to device addr=%d", dev->address);

	uint8_t config_buf[256];
	memset(config_buf, 0, sizeof(config_buf));

	usb_setup_packet_t setup;
	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = (USB_DESC_CONFIGURATION << 8) | 0;
	setup.wIndex = 0;
	setup.wLength = sizeof(config_buf);

	if (!hc->control_transfer(hc, dev, &setup, config_buf, sizeof(config_buf))) {
		debugf(WARNING, "usb_storage: failed to read configuration descriptor");
		return;
	}

	usb_configuration_descriptor_t* config = (usb_configuration_descriptor_t*)config_buf;

	usb_endpoint_descriptor_t* ep_in = NULL;
	usb_endpoint_descriptor_t* ep_out = NULL;
	usb_interface_descriptor_t* cur_iface = NULL;

	uint8_t* ptr = config_buf + config->bLength;
	uint8_t* end = config_buf + config->wTotalLength;
	if (end > config_buf + sizeof(config_buf)) {
		end = config_buf + sizeof(config_buf);
	}

	while (ptr < end) {
		uint8_t desc_len = ptr[0];
		uint8_t desc_type = ptr[1];

		if (desc_len == 0) {
			break;
		}

		if (desc_type == USB_DESC_INTERFACE) {
			cur_iface = (usb_interface_descriptor_t*)ptr;
		} else if (desc_type == USB_DESC_ENDPOINT && cur_iface != NULL) {
			if (cur_iface->bInterfaceClass == USB_CLASS_MASS_STORAGE &&
			    cur_iface->bInterfaceSubClass == USB_MSC_SUBCLASS_SCSI &&
			    cur_iface->bInterfaceProtocol == USB_MSC_PROTOCOL_BBB) {
				usb_endpoint_descriptor_t* endp = (usb_endpoint_descriptor_t*)ptr;
				uint8_t attr = endp->bmAttributes & 0x03;

				if (attr == USB_ENDPOINT_BULK) {
					if (endp->bEndpointAddress & 0x80) {
						ep_in = endp;
					} else {
						ep_out = endp;
					}
				}
			}
		}

		ptr += desc_len;
	}

	if (ep_in == NULL || ep_out == NULL) {
		debugf(WARNING, "usb_storage: could not find both bulk endpoints (in=%p, out=%p)", ep_in, ep_out);
		return;
	}

	debugf(INFO, "usb_storage: found bulk IN ep=0x%x maxpkt=%d, bulk OUT ep=0x%x maxpkt=%d", ep_in->bEndpointAddress, ep_in->wMaxPacketSize, ep_out->bEndpointAddress, ep_out->wMaxPacketSize);

	disk_driver_t* storage = usb_storage_create(hc, dev, ep_in, ep_out);
	register_driver((driver_t*)storage);
}

usb_device_driver_t usb_storage_device_driver = {
	.name = "usb_storage",
	.interface_class = USB_CLASS_MASS_STORAGE,
	.interface_subclass = USB_MSC_SUBCLASS_SCSI,
	.interface_protocol = USB_MSC_PROTOCOL_BBB,
	.attach = usb_storage_attach,
};

void main() {
}

void stage_driver() {
	register_usb_device_driver(&usb_storage_device_driver);
}

define_module("usb_storage", main, stage_driver, NULL);
