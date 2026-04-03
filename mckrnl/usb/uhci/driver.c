#include <uhci.h>

#include <string.h>
#include <memory/heap.h>
#include <memory/vmm.h>
#include <utils/io.h>
#include <driver/pci/pci_bar.h>
#include <stdio.h>

void uhci_outw(uhci_controller_t* hc, uint16_t reg, uint16_t val) {
	outw(hc->io_base + reg, val);
}

uint16_t uhci_inw(uhci_controller_t* hc, uint16_t reg) {
	return inw(hc->io_base + reg);
}

void uhci_outl(uhci_controller_t* hc, uint16_t reg, uint32_t val) {
	outl(hc->io_base + reg, val);
}

void uhci_outb(uhci_controller_t* hc, uint16_t reg, uint8_t val) {
	outb(hc->io_base + reg, val);
}

void uhci_delay(int ms) {
	for (volatile int i = 0; i < ms * 10000; i++) {
		__asm__ volatile("nop");
	}
}

bool uhci_is_device_present(driver_t* driver) {
	return true;
}

char* uhci_get_device_name(driver_t* driver) {
	return "uhci";
}

uhci_td_t* uhci_alloc_td(uhci_controller_t* hc) {
	for (int attempt = 0; attempt < UHCI_MAX_TDS; attempt++) {
		if (hc->next_td >= UHCI_MAX_TDS) {
			hc->next_td = 0;
		}
		uhci_td_t* td = &hc->td_pool[hc->next_td++];

		if (td->status & UHCI_TD_STATUS_ACTIVE) {
			continue;
		}
		memset(td, 0, sizeof(uhci_td_t));
		return td;
	}

	debugf(WARNING, "uhci: no free TDs available");
	return NULL;
}

uhci_qh_t* uhci_alloc_qh(uhci_controller_t* hc) {
	for (int attempt = 0; attempt < UHCI_MAX_QHS; attempt++) {
		if (hc->next_qh >= UHCI_MAX_QHS) {
			hc->next_qh = 0;
		}
		uhci_qh_t* qh = &hc->qh_pool[hc->next_qh++];

		if (qh == hc->qh_interrupt) {
			continue;
		}

		if (qh->element != 0 && !(qh->element & UHCI_TD_LINK_TERMINATE)) {
			uhci_td_t* td = (uhci_td_t*)(qh->element & ~0xF);
			if (td->status & UHCI_TD_STATUS_ACTIVE) {
				continue;
			}
		}
		memset(qh, 0, sizeof(uhci_qh_t));
		return qh;
	}
	debugf(WARNING, "uhci: no free QHs available");
	return NULL;
}

void uhci_reset(uhci_controller_t* hc) {
	uhci_outw(hc, UHCI_REG_USBCMD, UHCI_CMD_GRESET);
	uhci_delay(50);
	uhci_outw(hc, UHCI_REG_USBCMD, 0);
	uhci_delay(10);

	uhci_outw(hc, UHCI_REG_USBCMD, UHCI_CMD_HCRESET);
	uhci_delay(50);

	for (int i = 0; i < 100; i++) {
		if (!(uhci_inw(hc, UHCI_REG_USBCMD) & UHCI_CMD_HCRESET)) {
			break;
		}
		uhci_delay(1);
	}

	uhci_outw(hc, UHCI_REG_USBSTS, 0xFFFF);
}

void uhci_start(uhci_controller_t* hc) {
	uhci_outl(hc, UHCI_REG_FRBASEADD, (uint32_t)hc->frame_list);
	uhci_outw(hc, UHCI_REG_FRNUM, 0);
	uhci_outb(hc, UHCI_REG_SOFMOD, 0x40);
	uhci_outw(hc, UHCI_REG_USBINTR, 0);
	uhci_outw(hc, UHCI_REG_USBSTS, 0xFFFF);
	uhci_outw(hc, UHCI_REG_USBCMD, UHCI_CMD_RS | UHCI_CMD_CF | UHCI_CMD_MAXP);

	uhci_delay(10);
}

void uhci_stop(uhci_controller_t* hc) {
	uhci_outw(hc, UHCI_REG_USBCMD, 0);
	uhci_delay(10);
}

void uhci_port_reset(uhci_controller_t* hc, int port) {
	uint16_t reg = (port == 0) ? UHCI_REG_PORTSC1 : UHCI_REG_PORTSC2;

	uhci_outw(hc, reg, UHCI_PORT_RESET);
	uhci_delay(50);

	uhci_outw(hc, reg, 0);
	uhci_delay(10);

	for (int i = 0; i < 10; i++) {
		uhci_delay(10);

		uint16_t status = uhci_inw(hc, reg);

		if (status & (UHCI_PORT_CONNECTION_CHG | UHCI_PORT_ENABLE_CHG)) {
			uhci_outw(hc, reg, status | UHCI_PORT_CONNECTION_CHG | UHCI_PORT_ENABLE_CHG);
			continue;
		}

		if (status & UHCI_PORT_CONNECTION) {
			uhci_outw(hc, reg, UHCI_PORT_ENABLE);
			uhci_delay(10);
			break;
		}
	}
}

bool uhci_port_connected(uhci_controller_t* hc, int port) {
	uint16_t reg = (port == 0) ? UHCI_REG_PORTSC1 : UHCI_REG_PORTSC2;
	return (uhci_inw(hc, reg) & UHCI_PORT_CONNECTION) != 0;
}

uint8_t uhci_port_speed(uhci_controller_t* hc, int port) {
	uint16_t reg = (port == 0) ? UHCI_REG_PORTSC1 : UHCI_REG_PORTSC2;
	return (uhci_inw(hc, reg) & UHCI_PORT_LSDA) ? USB_SPEED_LOW : USB_SPEED_FULL;
}

void uhci_td_setup(uhci_td_t* td, uint8_t speed, uint8_t addr, uint8_t endp, uint8_t toggle, uint8_t pid, uint16_t len, void* data) {
	td->link = UHCI_TD_LINK_TERMINATE;

	td->status = UHCI_TD_STATUS_ACTIVE | (3 << 27);
	if (speed == USB_SPEED_LOW) {
		td->status |= UHCI_TD_STATUS_LS;
	}

	uint16_t max_len = (len > 0) ? (len - 1) : 0x7FF;
	td->token = (max_len << 21) | (toggle << 19) | (endp << 15) | (addr << 8) | pid;

	td->buffer = (uint32_t)data;
}

bool uhci_transfer_wait(uhci_controller_t* hc, uhci_td_t* td, int timeout_ms) {
	uint16_t start_frame = uhci_inw(hc, UHCI_REG_FRNUM) & 0x7FF;
	int max_spin = timeout_ms * 100000;

	for (int spin = 0; spin < max_spin; spin++) {
		if (!(td->status & UHCI_TD_STATUS_ACTIVE)) {
			if (td->status & UHCI_TD_STATUS_NAK) {
				return false;
			}
			if (td->status & UHCI_TD_ERROR_MASK) {
				debugf(WARNING, "uhci: TD error, status=0x%x", td->status);
				return false;
			}
			return true;
		}

		uint16_t current_frame = uhci_inw(hc, UHCI_REG_FRNUM) & 0x7FF;
		uint16_t elapsed = (current_frame - start_frame) & 0x7FF;
		if (elapsed >= (uint16_t)timeout_ms) {
			break;
		}

		__asm__ volatile("pause");
	}

	return false;
}

bool uhci_control_transfer_impl(uhci_controller_t* hc, usb_device_t* dev, usb_setup_packet_t* setup, void* data, uint16_t len) {
	uhci_qh_t* qh = uhci_alloc_qh(hc);
	uhci_td_t* td_setup = uhci_alloc_td(hc);
	uhci_td_t* td_data = NULL;
	uhci_td_t* td_status = uhci_alloc_td(hc);

	if (qh == NULL || td_setup == NULL || td_status == NULL) {
		debugf(WARNING, "uhci: failed to allocate TDs/QH for control transfer");
		return false;
	}

	uint8_t speed = dev->speed;
	uint8_t addr = dev->address;
	uint8_t max_pkt = dev->max_packet_size;
	if (max_pkt == 0) {
		max_pkt = 8;
	}

	uhci_td_setup(td_setup, speed, addr, 0, 0, USB_PID_SETUP, 8, (void*)setup);

	if (len > 0 && data != NULL) {
		td_data = uhci_alloc_td(hc);
		if (td_data == NULL) {
			debugf(WARNING, "uhci: failed to allocate data TD");
			return false;
		}
		uint8_t data_pid = (setup->bmRequestType & USB_REQ_DEVICE_TO_HOST) ? USB_PID_IN : USB_PID_OUT;
		uhci_td_setup(td_data, speed, addr, 0, 1, data_pid, len, data);

		td_setup->link = (uint32_t)td_data | UHCI_TD_LINK_DEPTH;
		td_data->link = (uint32_t)td_status | UHCI_TD_LINK_DEPTH;
	} else {
		td_setup->link = (uint32_t)td_status | UHCI_TD_LINK_DEPTH;
	}

	uint8_t status_pid;
	if (len > 0) {
		status_pid = (setup->bmRequestType & USB_REQ_DEVICE_TO_HOST) ? USB_PID_OUT : USB_PID_IN;
	} else {
		status_pid = USB_PID_IN;
	}
	uhci_td_setup(td_status, speed, addr, 0, 1, status_pid, 0, NULL);
	td_status->status |= UHCI_TD_STATUS_IOC;

	qh->head = UHCI_TD_LINK_TERMINATE;
	qh->element = (uint32_t)td_setup;

	uint32_t old_head = hc->qh_interrupt->head;
	qh->head = old_head;
	hc->qh_interrupt->head = (uint32_t)qh | UHCI_TD_LINK_QH;

	bool ok = uhci_transfer_wait(hc, td_setup, 50);
	if (ok && td_data != NULL) {
		ok = uhci_transfer_wait(hc, td_data, 50);
	}
	if (ok) {
		ok = uhci_transfer_wait(hc, td_status, 50);
	}

	hc->qh_interrupt->head = qh->head;

	td_setup->status &= ~UHCI_TD_STATUS_ACTIVE;
	if (td_data != NULL) {
		td_data->status &= ~UHCI_TD_STATUS_ACTIVE;
	}
	td_status->status &= ~UHCI_TD_STATUS_ACTIVE;

	return ok;
}

bool uhci_interrupt_transfer_impl(uhci_controller_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len) {
	uint16_t sts = uhci_inw(hc, UHCI_REG_USBSTS);
	if (sts) {
		uhci_outw(hc, UHCI_REG_USBSTS, sts);
	}

	if (sts & UHCI_STS_HCH) {
		debugf(WARNING, "uhci: controller halted (status=0x%x), full restart", sts);
		uhci_start(hc);

		uint16_t cmd = uhci_inw(hc, UHCI_REG_USBCMD);
		uint16_t sts2 = uhci_inw(hc, UHCI_REG_USBSTS);
		if (sts2 & UHCI_STS_HCH) {
			debugf(ERROR, "uhci: controller still halted after restart (cmd=0x%x sts=0x%x)", cmd, sts2);
			return false;
		}
	}

	uhci_qh_t* qh = uhci_alloc_qh(hc);
	uhci_td_t* td = uhci_alloc_td(hc);

	if (qh == NULL || td == NULL) {
		debugf(WARNING, "uhci: failed to allocate TD/QH for interrupt transfer");
		return false;
	}

	uint8_t speed = dev->speed;
	uint8_t addr = dev->address;
	uint8_t endp = ep->bEndpointAddress & 0x0F;

	uhci_td_setup(td, speed, addr, endp, dev->interrupt_toggle, USB_PID_IN, len, data);

	qh->head = UHCI_TD_LINK_TERMINATE;
	qh->element = (uint32_t)td;

	uint32_t saved_old_head = hc->qh_interrupt->head;
	qh->head = saved_old_head;
	hc->qh_interrupt->head = (uint32_t)qh | UHCI_TD_LINK_QH;

	bool ok = uhci_transfer_wait(hc, td, 8);

	hc->qh_interrupt->head = saved_old_head;
	td->status &= ~UHCI_TD_STATUS_ACTIVE;

	sts = uhci_inw(hc, UHCI_REG_USBSTS);
	if (sts) {
		uhci_outw(hc, UHCI_REG_USBSTS, sts);
	}

	if (ok) {
		dev->interrupt_toggle ^= 1;
	}

	return ok;
}

bool uhci_bulk_transfer_impl(uhci_controller_t* hc, usb_device_t* dev,
                             usb_endpoint_descriptor_t* ep, void* data, uint16_t len,
                             uint8_t* toggle) {
	// Clear any pending status bits
	uint16_t sts = uhci_inw(hc, UHCI_REG_USBSTS);
	if (sts) {
		uhci_outw(hc, UHCI_REG_USBSTS, sts);
	}

	// Check if controller has halted and restart if needed
	if (sts & UHCI_STS_HCH) {
		debugf(WARNING, "uhci: controller halted before bulk transfer, restarting");
		uhci_start(hc);
		uint16_t sts2 = uhci_inw(hc, UHCI_REG_USBSTS);
		if (sts2 & UHCI_STS_HCH) {
			debugf(ERROR, "uhci: controller still halted after restart");
			return false;
		}
	}

	uint8_t speed = dev->speed;
	uint8_t addr = dev->address;
	uint8_t endp = ep->bEndpointAddress & 0x0F;
	uint8_t pid = (ep->bEndpointAddress & 0x80) ? USB_PID_IN : USB_PID_OUT;
	uint16_t max_pkt = ep->wMaxPacketSize;
	if (max_pkt == 0) {
		max_pkt = 64;
	}

	uint16_t remaining = len;
	uint16_t offset = 0;
	int num_tds = 0;
	if (len == 0) {
		num_tds = 1;
	} else {
		num_tds = (len + max_pkt - 1) / max_pkt;
	}

	if (num_tds > UHCI_MAX_TDS / 2) {
		debugf(WARNING, "uhci: bulk transfer too large (%d TDs needed)", num_tds);
		return false;
	}

	uhci_qh_t* qh = uhci_alloc_qh(hc);
	if (qh == NULL) {
		debugf(WARNING, "uhci: failed to allocate QH for bulk transfer");
		return false;
	}

	uhci_td_t* tds[64];
	memset(tds, 0, sizeof(tds));
	if (num_tds > 64) {
		num_tds = 64;
	}

	uint8_t cur_toggle = *toggle;
	for (int i = 0; i < num_tds; i++) {
		tds[i] = uhci_alloc_td(hc);
		if (tds[i] == NULL) {
			debugf(WARNING, "uhci: failed to allocate TD %d for bulk transfer", i);
			for (int j = 0; j < i; j++) {
				tds[j]->status &= ~UHCI_TD_STATUS_ACTIVE;
			}
			return false;
		}

		uint16_t pkt_len = (remaining > max_pkt) ? max_pkt : remaining;
		uhci_td_setup(tds[i], speed, addr, endp, cur_toggle, pid, pkt_len, (uint8_t*)data + offset);

		cur_toggle ^= 1;
		offset += pkt_len;
		remaining -= pkt_len;

		if (i > 0) {
			tds[i - 1]->link = (uint32_t)tds[i] | UHCI_TD_LINK_DEPTH;
		}
	}
	tds[num_tds - 1]->link = UHCI_TD_LINK_TERMINATE;

	qh->head = UHCI_TD_LINK_TERMINATE;
	qh->element = (uint32_t)tds[0];

	uint32_t saved_old_head = hc->qh_interrupt->head;
	qh->head = saved_old_head;
	hc->qh_interrupt->head = (uint32_t)qh | UHCI_TD_LINK_QH;

	bool ok = true;
	for (int i = 0; i < num_tds; i++) {
		if (!uhci_transfer_wait(hc, tds[i], 500)) {
			ok = false;
			break;
		}
	}

	hc->qh_interrupt->head = saved_old_head;

	for (int i = 0; i < num_tds; i++) {
		tds[i]->status &= ~UHCI_TD_STATUS_ACTIVE;
	}

	sts = uhci_inw(hc, UHCI_REG_USBSTS);
	if (sts) {
		uhci_outw(hc, UHCI_REG_USBSTS, sts);
	}

	if (ok) {
		*toggle = cur_toggle;
	}

	return ok;
}

bool uhci_hc_control_transfer(usb_hc_driver_t* hc_drv, usb_device_t* dev, usb_setup_packet_t* setup, void* data, uint16_t len) {
	uhci_controller_t* hc = (uhci_controller_t*)hc_drv->driver.driver_specific_data;
	return uhci_control_transfer_impl(hc, dev, setup, data, len);
}

bool uhci_hc_interrupt_transfer(usb_hc_driver_t* hc_drv, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len) {
	uhci_controller_t* hc = (uhci_controller_t*)hc_drv->driver.driver_specific_data;
	return uhci_interrupt_transfer_impl(hc, dev, ep, data, len);
}

bool uhci_hc_bulk_transfer(usb_hc_driver_t* hc_drv, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len, uint8_t* toggle) {
	uhci_controller_t* hc = (uhci_controller_t*)hc_drv->driver.driver_specific_data;
	return uhci_bulk_transfer_impl(hc, dev, ep, data, len, toggle);
}

void uhci_enumerate_hub(uhci_controller_t* hc, usb_device_t* hub_dev);

bool uhci_enumerate_usb_device(uhci_controller_t* hc, uint8_t speed) {
	usb_device_t temp_dev;
	memset(&temp_dev, 0, sizeof(usb_device_t));
	temp_dev.address = 0;
	temp_dev.speed = speed;
	temp_dev.max_packet_size = 8;

	usb_device_descriptor_t dev_desc;
	memset(&dev_desc, 0, sizeof(usb_device_descriptor_t));

	usb_setup_packet_t setup;
	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = (USB_DESC_DEVICE << 8) | 0;
	setup.wIndex = 0;
	setup.wLength = 8;

	if (!uhci_control_transfer_impl(hc, &temp_dev, &setup, &dev_desc, 8)) {
		debugf(WARNING, "uhci: failed to get device descriptor (first 8 bytes)");
		return false;
	}

	temp_dev.max_packet_size = dev_desc.bMaxPacketSize0;
	debugf(INFO, "uhci: max packet size: %d", temp_dev.max_packet_size);

	uint8_t new_addr = hc->next_address++;
	if (new_addr == 0) {
		new_addr = hc->next_address++;
	}

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_SET_ADDRESS;
	setup.wValue = new_addr;
	setup.wIndex = 0;
	setup.wLength = 0;

	if (!uhci_control_transfer_impl(hc, &temp_dev, &setup, NULL, 0)) {
		debugf(WARNING, "uhci: failed to set address %d", new_addr);
		return false;
	}

	uhci_delay(10);
	temp_dev.address = new_addr;
	debugf(INFO, "uhci: device assigned address %d", new_addr);

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = (USB_DESC_DEVICE << 8) | 0;
	setup.wIndex = 0;
	setup.wLength = sizeof(usb_device_descriptor_t);

	if (!uhci_control_transfer_impl(hc, &temp_dev, &setup, &dev_desc, sizeof(usb_device_descriptor_t))) {
		debugf(WARNING, "uhci: failed to get full device descriptor");
		return false;
	}

	debugf(INFO, "uhci: device vendor=0x%x product=0x%x class=0x%x", dev_desc.idVendor, dev_desc.idProduct, dev_desc.bDeviceClass);

	uint8_t config_buf[256];
	memset(config_buf, 0, sizeof(config_buf));

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = (USB_DESC_CONFIGURATION << 8) | 0;
	setup.wIndex = 0;
	setup.wLength = sizeof(config_buf);

	if (!uhci_control_transfer_impl(hc, &temp_dev, &setup, config_buf, sizeof(config_buf))) {
		debugf(WARNING, "uhci: failed to get configuration descriptor");
		return false;
	}

	usb_configuration_descriptor_t* config = (usb_configuration_descriptor_t*)config_buf;
	debugf(INFO, "uhci: config: num_interfaces=%d", config->bNumInterfaces);

	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_SET_CONFIGURATION;
	setup.wValue = config->bConfigurationValue;
	setup.wIndex = 0;
	setup.wLength = 0;

	if (!uhci_control_transfer_impl(hc, &temp_dev, &setup, NULL, 0)) {
		debugf(WARNING, "uhci: failed to set configuration");
		return false;
	}

	uint8_t* ptr = config_buf + config->bLength;
	uint8_t* end = config_buf + config->wTotalLength;

	usb_interface_descriptor_t* iface = NULL;
	usb_endpoint_descriptor_t* endp = NULL;

	while (ptr < end) {
		uint8_t desc_len = ptr[0];
		uint8_t desc_type = ptr[1];

		if (desc_len == 0) {
			break;
		}

		if (desc_type == USB_DESC_INTERFACE) {
			iface = (usb_interface_descriptor_t*)ptr;
			debugf(INFO, "uhci: interface class=0x%x subclass=0x%x protocol=0x%x", iface->bInterfaceClass, iface->bInterfaceSubClass, iface->bInterfaceProtocol);
		} else if (desc_type == USB_DESC_ENDPOINT) {
			endp = (usb_endpoint_descriptor_t*)ptr;
			debugf(INFO, "uhci: endpoint addr=0x%x attr=0x%x maxpkt=%d interval=%d", endp->bEndpointAddress, endp->bmAttributes, endp->wMaxPacketSize, endp->bInterval);
		} else {
			debugf(SPAM, "uhci: unknown descriptor type 0x%x", desc_type);
		}

		ptr += desc_len;
	}

	int dev_idx = -1;
	for (int i = 0; i < USB_MAX_DEVICES; i++) {
		if (!hc->devices[i].present) {
			dev_idx = i;
			break;
		}
	}

	if (dev_idx < 0) {
		debugf(WARNING, "uhci: no free device slots");
		return false;
	}

	usb_device_t* udev = &hc->devices[dev_idx];
	udev->present = true;
	udev->address = new_addr;
	udev->speed = speed;
	udev->port = 0;
	udev->max_packet_size = dev_desc.bMaxPacketSize0;
	udev->device_desc = dev_desc;
	udev->hc = hc->hc_driver;

	if (config != NULL) {
		memcpy(&udev->config_desc, config, sizeof(usb_configuration_descriptor_t));
	}
	if (iface != NULL) {
		memcpy(&udev->interface_desc, iface, sizeof(usb_interface_descriptor_t));
	}
	if (endp != NULL) {
		memcpy(&udev->endpoint_desc, endp, sizeof(usb_endpoint_descriptor_t));
	}

	if (dev_desc.bDeviceClass == USB_CLASS_HUB) {
		debugf(INFO, "uhci: device is a hub, enumerating downstream ports");
		uhci_enumerate_hub(hc, udev);
		return true;
	}

	if (iface != NULL) {
		usb_device_attached(hc->hc_driver, udev, iface, endp);
	}

	return true;
}

bool uhci_enumerate_device(uhci_controller_t* hc, int port) {
	debugf(INFO, "uhci: enumerating device on port %d", port);

	uhci_port_reset(hc, port);
	uhci_delay(20);

	if (!uhci_port_connected(hc, port)) {
		debugf(INFO, "uhci: no device on port %d after reset", port);
		return false;
	}

	uint8_t speed = uhci_port_speed(hc, port);
	debugf(INFO, "uhci: port %d device speed: %s", port, speed == USB_SPEED_LOW ? "low" : "full");

	return uhci_enumerate_usb_device(hc, speed);
}

void uhci_enumerate_hub(uhci_controller_t* hc, usb_device_t* hub_dev) {
	debugf(INFO, "uhci: probing hub at address %d", hub_dev->address);

	usb_hub_descriptor_t hub_desc;
	memset(&hub_desc, 0, sizeof(hub_desc));

	usb_setup_packet_t setup;
	memset(&setup, 0, sizeof(usb_setup_packet_t));
	setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_DEVICE;
	setup.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.wValue = (USB_DESC_HUB << 8) | 0;
	setup.wIndex = 0;
	setup.wLength = sizeof(hub_desc);

	if (!uhci_control_transfer_impl(hc, hub_dev, &setup, &hub_desc, sizeof(hub_desc))) {
		debugf(WARNING, "uhci: failed to get hub descriptor");
		return;
	}

	int num_ports = hub_desc.bNbrPorts;
	debugf(INFO, "uhci: hub has %d ports, power-on delay %d ms",
	       num_ports, hub_desc.bPwrOn2PwrGood * 2);

	for (int port = 1; port <= num_ports; port++) {
		memset(&setup, 0, sizeof(usb_setup_packet_t));
		setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
		setup.bRequest = USB_REQ_SET_FEATURE;
		setup.wValue = USB_HUB_FEAT_PORT_POWER;
		setup.wIndex = port;
		setup.wLength = 0;
		uhci_control_transfer_impl(hc, hub_dev, &setup, NULL, 0);
	}

	uhci_delay(hub_desc.bPwrOn2PwrGood * 2 + 100);

	for (int port = 1; port <= num_ports; port++) {
		usb_hub_port_status_t port_status;
		memset(&port_status, 0, sizeof(port_status));

		memset(&setup, 0, sizeof(usb_setup_packet_t));
		setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
		setup.bRequest = USB_REQ_GET_STATUS;
		setup.wValue = 0;
		setup.wIndex = port;
		setup.wLength = sizeof(port_status);

		if (!uhci_control_transfer_impl(hc, hub_dev, &setup, &port_status, sizeof(port_status))) {
			debugf(WARNING, "uhci: hub port %d: failed to get status", port);
			continue;
		}

		debugf(INFO, "uhci: hub port %d: status=0x%x change=0x%x", port, port_status.wPortStatus, port_status.wPortChange);

		if (!(port_status.wPortStatus & USB_HUB_PORT_STAT_CONNECTION)) {
			continue;
		}

		debugf(INFO, "uhci: hub port %d: device connected", port);

		memset(&setup, 0, sizeof(usb_setup_packet_t));
		setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
		setup.bRequest = USB_REQ_CLEAR_FEATURE;
		setup.wValue = USB_HUB_FEAT_C_PORT_CONNECTION;
		setup.wIndex = port;
		setup.wLength = 0;
		uhci_control_transfer_impl(hc, hub_dev, &setup, NULL, 0);

		memset(&setup, 0, sizeof(usb_setup_packet_t));
		setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
		setup.bRequest = USB_REQ_SET_FEATURE;
		setup.wValue = USB_HUB_FEAT_PORT_RESET;
		setup.wIndex = port;
		setup.wLength = 0;

		if (!uhci_control_transfer_impl(hc, hub_dev, &setup, NULL, 0)) {
			debugf(WARNING, "uhci: hub port %d: failed to start reset", port);
			continue;
		}

		uhci_delay(50);
		bool reset_done = false;

		for (int attempt = 0; attempt < 10; attempt++) {
			memset(&port_status, 0, sizeof(port_status));
			memset(&setup, 0, sizeof(usb_setup_packet_t));
			setup.bmRequestType = USB_REQ_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
			setup.bRequest = USB_REQ_GET_STATUS;
			setup.wValue = 0;
			setup.wIndex = port;
			setup.wLength = sizeof(port_status);

			if (!uhci_control_transfer_impl(hc, hub_dev, &setup, &port_status, sizeof(port_status))) {
				break;
			}

			if (!(port_status.wPortStatus & USB_HUB_PORT_STAT_RESET)) {
				reset_done = true;
				break;
			}
			uhci_delay(20);
		}

		if (!reset_done) {
			debugf(WARNING, "uhci: hub port %d: reset did not complete", port);
			continue;
		}

		if (!(port_status.wPortStatus & USB_HUB_PORT_STAT_ENABLE)) {
			debugf(WARNING, "uhci: hub port %d: not enabled after reset", port);
			continue;
		}

		memset(&setup, 0, sizeof(usb_setup_packet_t));
		setup.bmRequestType = USB_REQ_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER;
		setup.bRequest = USB_REQ_CLEAR_FEATURE;
		setup.wValue = USB_HUB_FEAT_C_PORT_RESET;
		setup.wIndex = port;
		setup.wLength = 0;
		uhci_control_transfer_impl(hc, hub_dev, &setup, NULL, 0);

		uhci_delay(10);

		uint8_t speed = (port_status.wPortStatus & USB_HUB_PORT_STAT_LOW_SPEED) ? USB_SPEED_LOW : USB_SPEED_FULL;

		debugf(INFO, "uhci: hub port %d: enumerating %s-speed device", port, speed == USB_SPEED_LOW ? "low" : "full");

		uhci_enumerate_usb_device(hc, speed);
	}
}

void uhci_probe_ports(uhci_controller_t* hc) {
	for (int port = 0; port < hc->num_ports; port++) {
		if (uhci_port_connected(hc, port)) {
			debugf(INFO, "uhci: device connected on port %d", port);
			uhci_enumerate_device(hc, port);
		} else {
			debugf(SPAM, "uhci: no device on port %d", port);
		}
	}
}

cpu_registers_t* uhci_interrupt(cpu_registers_t* regs, void* data) {
	uhci_controller_t* hc = (uhci_controller_t*)data;

	uint16_t status = uhci_inw(hc, UHCI_REG_USBSTS);

	uhci_outw(hc, UHCI_REG_USBSTS, status);

	if (status & UHCI_STS_ERROR) {
		debugf(WARNING, "uhci: USB error interrupt");
	}

	if (status & UHCI_STS_HSE) {
		debugf(ERROR, "uhci: host system error!");
	}

	return regs;
}

void uhci_init(driver_t* driver) {
	usb_hc_driver_t* hc_drv = (usb_hc_driver_t*)driver;
	uhci_controller_t* hc = (uhci_controller_t*)driver->driver_specific_data;

	debugf(INFO, "uhci: initializing controller io_base=0x%x irq=%d", hc->io_base, hc->pci_dev.header.interrupt_line);

	enable_io(hc->pci_dev.bus, hc->pci_dev.device, hc->pci_dev.function);
	enable_bus_master(hc->pci_dev.bus, hc->pci_dev.device, hc->pci_dev.function);
	enable_interrupt(hc->pci_dev.bus, hc->pci_dev.device, hc->pci_dev.function);

	hc->frame_list = (uint32_t*)vmm_alloc(1);
	memset(hc->frame_list, 0, UHCI_FRAME_LIST_SIZE * sizeof(uint32_t));

	hc->td_pool = (uhci_td_t*)vmm_alloc(TO_PAGES(sizeof(uhci_td_t) * UHCI_MAX_TDS));
	memset(hc->td_pool, 0, sizeof(uhci_td_t) * UHCI_MAX_TDS);
	hc->next_td = 0;

	hc->qh_pool = (uhci_qh_t*)vmm_alloc(TO_PAGES(sizeof(uhci_qh_t) * UHCI_MAX_QHS));
	memset(hc->qh_pool, 0, sizeof(uhci_qh_t) * UHCI_MAX_QHS);
	hc->next_qh = 0;

	hc->qh_interrupt = uhci_alloc_qh(hc);
	hc->qh_interrupt->head = UHCI_TD_LINK_TERMINATE;
	hc->qh_interrupt->element = UHCI_TD_LINK_TERMINATE;

	for (int i = 0; i < UHCI_FRAME_LIST_SIZE; i++) {
		hc->frame_list[i] = (uint32_t)hc->qh_interrupt | UHCI_TD_LINK_QH;
	}

	hc->next_address = 1;
	hc->num_ports = 2;
	hc->hc_driver = hc_drv;

	register_interrupt_handler(hc->pci_dev.header.interrupt_line + 0x20, uhci_interrupt, hc);

	uhci_reset(hc);
	uhci_start(hc);

	debugf(INFO, "uhci: controller started, probing ports...");

	uhci_probe_ports(hc);

	debugf(INFO, "uhci: initialization complete");
}

void uhci_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
	debugf(INFO, "uhci: found UHCI controller at %d:%d.%d vendor=0x%x device=0x%x", bus, device, (int)function, header.vendor_id, header.device_id);

	pci_bar_t bar = pci_get_bar(&header.BAR0, 4, bus, device, function);
	uint16_t io_base = 0;

	if (bar.type == IO) {
		io_base = bar.io_address;
	} else {
		for (int i = 0; i < 6; i++) {
			bar = pci_get_bar(&header.BAR0, i, bus, device, function);
			if (bar.type == IO) {
				io_base = bar.io_address;
				break;
			}
		}
	}

	if (io_base == 0) {
		debugf(WARNING, "uhci: could not find I/O base address");
		return;
	}

	usb_hc_driver_t* hc_drv = kmalloc(sizeof(usb_hc_driver_t));
	memset(hc_drv, 0, sizeof(usb_hc_driver_t));

	uhci_controller_t* hc = kmalloc(sizeof(uhci_controller_t));
	memset(hc, 0, sizeof(uhci_controller_t));

	hc->pci_dev.header = header;
	hc->pci_dev.bus = bus;
	hc->pci_dev.device = device;
	hc->pci_dev.function = function;
	hc->io_base = io_base;

	hc_drv->driver.is_device_present = uhci_is_device_present;
	hc_drv->driver.get_device_name = uhci_get_device_name;
	hc_drv->driver.init = uhci_init;
	hc_drv->driver.driver_specific_data = hc;
	hc_drv->control_transfer = uhci_hc_control_transfer;
	hc_drv->interrupt_transfer = uhci_hc_interrupt_transfer;
	hc_drv->bulk_transfer = uhci_hc_bulk_transfer;

	register_driver((driver_t*)hc_drv);
}
