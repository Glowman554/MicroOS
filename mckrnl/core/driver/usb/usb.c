#include <driver/usb.h>
#include <stdio.h>
#include <stddef.h>

static usb_device_driver_t* usb_device_drivers[USB_MAX_DEVICE_DRIVERS];
static int num_usb_device_drivers = 0;

void register_usb_device_driver(usb_device_driver_t* drv) {
	if (num_usb_device_drivers >= USB_MAX_DEVICE_DRIVERS) {
		debugf(WARNING, "usb: too many USB device drivers registered");
		return;
	}

	usb_device_drivers[num_usb_device_drivers++] = drv;
	debugf(SPAM, "usb: registered device driver '%s' (class=0x%x sub=0x%x proto=0x%x)", drv->name, drv->interface_class, drv->interface_subclass, drv->interface_protocol);
}

void usb_device_attached(usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep) {
	if (iface == NULL) {
		return;
	}

	debugf(INFO, "usb: device attached (class=0x%x sub=0x%x proto=0x%x)", iface->bInterfaceClass, iface->bInterfaceSubClass, iface->bInterfaceProtocol);

	for (int i = 0; i < num_usb_device_drivers; i++) {
		usb_device_driver_t* drv = usb_device_drivers[i];

		if (drv->interface_class == iface->bInterfaceClass &&
		    drv->interface_subclass == iface->bInterfaceSubClass &&
		    drv->interface_protocol == iface->bInterfaceProtocol) {
			debugf(INFO, "usb: found device driver '%s'", drv->name);
			drv->attach(drv, hc, dev, iface, ep);
			return;
		}
	}

	debugf(WARNING, "usb: no driver found for device (class=0x%x sub=0x%x proto=0x%x)", iface->bInterfaceClass, iface->bInterfaceSubClass, iface->bInterfaceProtocol);
}
