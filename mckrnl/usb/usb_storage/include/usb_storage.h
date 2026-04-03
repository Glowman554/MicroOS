#pragma once

#include <driver/usb.h>
#include <driver/disk_driver.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct usb_msc_cbw {
	uint32_t dCBWSignature;
	uint32_t dCBWTag;
	uint32_t dCBWDataTransferLength;
	uint8_t  bmCBWFlags;
	uint8_t  bCBWLUN;
	uint8_t  bCBWCBLength;
	uint8_t  CBWCB[16];
} __attribute__((packed)) usb_msc_cbw_t;

typedef struct usb_msc_csw {
	uint32_t dCSWSignature;
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;
	uint8_t  bCSWStatus;
} __attribute__((packed)) usb_msc_csw_t;

#define USB_MSC_CBW_SIGNATURE  0x43425355
#define USB_MSC_CSW_SIGNATURE  0x53425355

#define USB_MSC_CBW_FLAG_IN    0x80
#define USB_MSC_CBW_FLAG_OUT   0x00

#define USB_MSC_CSW_STATUS_PASSED      0
#define USB_MSC_CSW_STATUS_FAILED      1
#define USB_MSC_CSW_STATUS_PHASE_ERROR 2

#define SCSI_TEST_UNIT_READY  0x00
#define SCSI_INQUIRY          0x12
#define SCSI_READ_CAPACITY_10 0x25
#define SCSI_READ_10          0x28
#define SCSI_WRITE_10         0x2A

typedef struct scsi_read_capacity_10_data {
	uint32_t last_lba;
	uint32_t block_size;
} __attribute__((packed)) scsi_read_capacity_10_data_t;

typedef struct scsi_inquiry_data {
	uint8_t peripheral;
	uint8_t removable;
	uint8_t version;
	uint8_t response_format;
	uint8_t additional_length;
	uint8_t reserved[3];
	char vendor[8];
	char product[16];
	char revision[4];
} __attribute__((packed)) scsi_inquiry_data_t;

typedef struct usb_storage_data {
	usb_hc_driver_t* hc;
	usb_device_t* dev;
	usb_endpoint_descriptor_t ep_in;
	usb_endpoint_descriptor_t ep_out;
	uint8_t toggle_in;
	uint8_t toggle_out;
	uint32_t tag;
	uint32_t num_blocks;
	uint32_t block_size;
	uint8_t lun;
} usb_storage_data_t;

disk_driver_t* usb_storage_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep_in, usb_endpoint_descriptor_t* ep_out);

bool usb_storage_scsi_command(usb_storage_data_t* data, uint8_t* cdb, uint8_t cdb_len, void* buf, uint32_t buf_len, bool direction_in);
