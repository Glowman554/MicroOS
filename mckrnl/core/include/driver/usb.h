#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <driver/driver.h>

#define USB_REQ_HOST_TO_DEVICE    0x00
#define USB_REQ_DEVICE_TO_HOST    0x80
#define USB_REQ_TYPE_STANDARD     0x00
#define USB_REQ_TYPE_CLASS        0x20
#define USB_REQ_TYPE_VENDOR       0x40
#define USB_REQ_RECIP_DEVICE      0x00
#define USB_REQ_RECIP_INTERFACE   0x01
#define USB_REQ_RECIP_ENDPOINT    0x02
#define USB_REQ_RECIP_OTHER       0x03

#define USB_REQ_GET_STATUS        0x00
#define USB_REQ_CLEAR_FEATURE     0x01
#define USB_REQ_SET_FEATURE       0x03
#define USB_REQ_SET_ADDRESS       0x05
#define USB_REQ_GET_DESCRIPTOR    0x06
#define USB_REQ_SET_DESCRIPTOR    0x07
#define USB_REQ_GET_CONFIGURATION 0x08
#define USB_REQ_SET_CONFIGURATION 0x09
#define USB_REQ_GET_INTERFACE     0x0A
#define USB_REQ_SET_INTERFACE     0x0B

#define USB_DESC_DEVICE           0x01
#define USB_DESC_CONFIGURATION    0x02
#define USB_DESC_STRING           0x03
#define USB_DESC_INTERFACE        0x04
#define USB_DESC_ENDPOINT         0x05
#define USB_DESC_HID              0x21
#define USB_DESC_HID_REPORT       0x22

#define USB_CLASS_HID             0x03
#define USB_CLASS_MASS_STORAGE    0x08

#define USB_MSC_SUBCLASS_SCSI     0x06

#define USB_MSC_PROTOCOL_BBB      0x50

#define USB_CLASS_HUB             0x09

#define USB_DESC_HUB              0x29

#define USB_HUB_FEAT_PORT_RESET          4
#define USB_HUB_FEAT_PORT_POWER          8
#define USB_HUB_FEAT_C_PORT_CONNECTION   16
#define USB_HUB_FEAT_C_PORT_RESET        20

#define USB_HUB_PORT_STAT_CONNECTION     (1 << 0)
#define USB_HUB_PORT_STAT_ENABLE         (1 << 1)
#define USB_HUB_PORT_STAT_RESET          (1 << 4)
#define USB_HUB_PORT_STAT_POWER          (1 << 8)
#define USB_HUB_PORT_STAT_LOW_SPEED      (1 << 9)

#define USB_HID_SUBCLASS_BOOT    0x01

#define USB_HID_PROTOCOL_KEYBOARD 0x01
#define USB_HID_PROTOCOL_MOUSE    0x02

#define USB_HID_GET_REPORT        0x01
#define USB_HID_SET_REPORT        0x09
#define USB_HID_SET_IDLE          0x0A
#define USB_HID_SET_PROTOCOL      0x0B

#define USB_PID_SETUP             0x2D
#define USB_PID_IN                0x69
#define USB_PID_OUT               0xE1

#define USB_ENDPOINT_CONTROL      0x00
#define USB_ENDPOINT_ISOCHRONOUS  0x01
#define USB_ENDPOINT_BULK         0x02
#define USB_ENDPOINT_INTERRUPT    0x03

#define USB_SPEED_LOW             0
#define USB_SPEED_FULL            1

typedef struct usb_setup_packet {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__((packed)) usb_setup_packet_t;

typedef struct usb_device_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
} __attribute__((packed)) usb_device_descriptor_t;

typedef struct usb_configuration_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
} __attribute__((packed)) usb_configuration_descriptor_t;

typedef struct usb_interface_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__((packed)) usb_interface_descriptor_t;

typedef struct usb_endpoint_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
} __attribute__((packed)) usb_endpoint_descriptor_t;

typedef struct usb_hub_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bNbrPorts;
	uint16_t wHubCharacteristics;
	uint8_t bPwrOn2PwrGood;
	uint8_t bHubContrCurrent;
} __attribute__((packed)) usb_hub_descriptor_t;

typedef struct usb_hub_port_status {
	uint16_t wPortStatus;
	uint16_t wPortChange;
} __attribute__((packed)) usb_hub_port_status_t;

typedef struct usb_hid_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdHID;
	uint8_t bCountryCode;
	uint8_t bNumDescriptors;
	uint8_t bClassDescriptorType;
	uint16_t wDescriptorLength;
} __attribute__((packed)) usb_hid_descriptor_t;

typedef struct usb_keyboard_report {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t keys[6];
} __attribute__((packed)) usb_keyboard_report_t;

#define USB_KB_MOD_LEFT_CTRL   (1 << 0)
#define USB_KB_MOD_LEFT_SHIFT  (1 << 1)
#define USB_KB_MOD_LEFT_ALT    (1 << 2)
#define USB_KB_MOD_LEFT_GUI    (1 << 3)
#define USB_KB_MOD_RIGHT_CTRL  (1 << 4)
#define USB_KB_MOD_RIGHT_SHIFT (1 << 5)
#define USB_KB_MOD_RIGHT_ALT   (1 << 6)
#define USB_KB_MOD_RIGHT_GUI   (1 << 7)

#define USB_MAX_DEVICES 16

struct usb_hc_driver;

typedef struct usb_device {
	bool present;
	uint8_t address;
	uint8_t speed;
	uint8_t port;
	uint8_t max_packet_size;
	usb_device_descriptor_t device_desc;
	usb_configuration_descriptor_t config_desc;
	usb_interface_descriptor_t interface_desc;
	usb_endpoint_descriptor_t endpoint_desc;
	struct usb_hc_driver* hc;
	uint8_t interrupt_toggle;
} usb_device_t;

typedef struct usb_hc_driver {
	driver_t driver;

	bool (*control_transfer)(struct usb_hc_driver* hc, usb_device_t* dev, usb_setup_packet_t* setup, void* data, uint16_t len);
	bool (*interrupt_transfer)(struct usb_hc_driver* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len);
	bool (*bulk_transfer)(struct usb_hc_driver* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep, void* data, uint16_t len, uint8_t* toggle);
} usb_hc_driver_t;

#define USB_MAX_DEVICE_DRIVERS 16

typedef struct usb_device_driver {
	const char* name;

	uint8_t interface_class;
	uint8_t interface_subclass;
	uint8_t interface_protocol;

	void (*attach)(struct usb_device_driver* drv, usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep);
} usb_device_driver_t;


void register_usb_device_driver(usb_device_driver_t* drv);
void usb_device_attached(usb_hc_driver_t* hc, usb_device_t* dev, usb_interface_descriptor_t* iface, usb_endpoint_descriptor_t* ep);
