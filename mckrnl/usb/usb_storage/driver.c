#include <usb_storage.h>

#include <string.h>
#include <memory/heap.h>
#include <stdio.h>
#include <fs/gpt.h>

uint32_t bswap32(uint32_t val) {
	return ((val & 0xFF) << 24) |
	       ((val & 0xFF00) << 8) |
	       ((val & 0xFF0000) >> 8) |
	       ((val & 0xFF000000) >> 24);
}

bool usb_storage_scsi_command(usb_storage_data_t* data, uint8_t* cdb, uint8_t cdb_len, void* buf, uint32_t buf_len, bool direction_in) {
	usb_msc_cbw_t cbw;
	memset(&cbw, 0, sizeof(usb_msc_cbw_t));
	cbw.dCBWSignature = USB_MSC_CBW_SIGNATURE;
	cbw.dCBWTag = data->tag++;
	cbw.dCBWDataTransferLength = buf_len;
	cbw.bmCBWFlags = direction_in ? USB_MSC_CBW_FLAG_IN : USB_MSC_CBW_FLAG_OUT;
	cbw.bCBWLUN = data->lun;
	cbw.bCBWCBLength = cdb_len;
	memcpy(cbw.CBWCB, cdb, cdb_len);

	if (!data->hc->bulk_transfer(data->hc, data->dev, &data->ep_out, &cbw, sizeof(usb_msc_cbw_t), &data->toggle_out)) {
		debugf(WARNING, "usb_storage: failed to send CBW");
		return false;
	}

	if (buf_len > 0 && buf != NULL) {
		usb_endpoint_descriptor_t* ep = direction_in ? &data->ep_in : &data->ep_out;
		uint8_t* toggle = direction_in ? &data->toggle_in : &data->toggle_out;

		uint32_t offset = 0;
		while (offset < buf_len) {
			uint16_t chunk = buf_len - offset;
			uint16_t max_chunk = ep->wMaxPacketSize * 64;
			if (max_chunk == 0) {
				max_chunk = 64 * 64;
			}
			if (chunk > max_chunk) {
				chunk = max_chunk;
			}

			if (!data->hc->bulk_transfer(data->hc, data->dev, ep, (uint8_t*)buf + offset, chunk, toggle)) {
				debugf(WARNING, "usb_storage: data phase failed at offset %d", offset);
				return false;
			}
			offset += chunk;
		}
	}

	usb_msc_csw_t csw;
	memset(&csw, 0, sizeof(usb_msc_csw_t));

	if (!data->hc->bulk_transfer(data->hc, data->dev, &data->ep_in, &csw, sizeof(usb_msc_csw_t), &data->toggle_in)) {
		debugf(WARNING, "usb_storage: failed to receive CSW");
		return false;
	}

	if (csw.dCSWSignature != USB_MSC_CSW_SIGNATURE) {
		debugf(WARNING, "usb_storage: invalid CSW signature 0x%x", csw.dCSWSignature);
		return false;
	}

	if (csw.dCSWTag != cbw.dCBWTag) {
		debugf(WARNING, "usb_storage: CSW tag mismatch (expected %d, got %d)", cbw.dCBWTag, csw.dCSWTag);
		return false;
	}

	if (csw.bCSWStatus != USB_MSC_CSW_STATUS_PASSED) {
		debugf(WARNING, "usb_storage: command failed, status=%d", csw.bCSWStatus);
		return false;
	}

	return true;
}

bool usb_storage_test_unit_ready(usb_storage_data_t* data) {
	uint8_t cdb[6];
	memset(cdb, 0, sizeof(cdb));
	cdb[0] = SCSI_TEST_UNIT_READY;

	return usb_storage_scsi_command(data, cdb, 6, NULL, 0, true);
}

bool usb_storage_inquiry(usb_storage_data_t* data) {
	uint8_t cdb[6];
	memset(cdb, 0, sizeof(cdb));
	cdb[0] = SCSI_INQUIRY;
	cdb[4] = 36;

	scsi_inquiry_data_t inquiry;
	memset(&inquiry, 0, sizeof(inquiry));

	if (!usb_storage_scsi_command(data, cdb, 6, &inquiry, 36, true)) {
		return false;
	}

	debugf(INFO, "usb_storage: INQUIRY vendor=%s product=%s", inquiry.vendor, inquiry.product);
	return true;
}

bool usb_storage_read_capacity(usb_storage_data_t* data) {
	uint8_t cdb[10];
	memset(cdb, 0, sizeof(cdb));
	cdb[0] = SCSI_READ_CAPACITY_10;

	scsi_read_capacity_10_data_t cap;
	memset(&cap, 0, sizeof(cap));

	if (!usb_storage_scsi_command(data, cdb, 10, &cap, sizeof(cap), true)) {
		return false;
	}

	data->num_blocks = bswap32(cap.last_lba) + 1;
	data->block_size = bswap32(cap.block_size);

	debugf(INFO, "usb_storage: capacity: %d blocks, %d bytes/block", data->num_blocks, data->block_size);

	return true;
}

bool usb_storage_is_device_present(driver_t* driver) {
	return true;
}

char* usb_storage_get_device_name(driver_t* driver) {
	return "usb_storage";
}

void usb_storage_flush(disk_driver_t* driver) {
}

void usb_storage_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	usb_storage_data_t* data = (usb_storage_data_t*)driver->driver.driver_specific_data;

	for (uint32_t i = 0; i < count; i++) {
		uint32_t lba = (uint32_t)(sector + i);

		uint8_t cdb[10];
		memset(cdb, 0, sizeof(cdb));
		cdb[0] = SCSI_READ_10;
		cdb[2] = (lba >> 24) & 0xFF;
		cdb[3] = (lba >> 16) & 0xFF;
		cdb[4] = (lba >> 8) & 0xFF;
		cdb[5] = lba & 0xFF;
		cdb[7] = 0;
		cdb[8] = 1;

		if (!usb_storage_scsi_command(data, cdb, 10, (uint8_t*)buffer + i * data->block_size, data->block_size, true)) {
			debugf(ERROR, "usb_storage: read failed at sector %d", lba);
			return;
		}
	}
}

void usb_storage_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
	usb_storage_data_t* data = (usb_storage_data_t*)driver->driver.driver_specific_data;

	for (uint32_t i = 0; i < count; i++) {
		uint32_t lba = (uint32_t)(sector + i);

		uint8_t cdb[10];
		memset(cdb, 0, sizeof(cdb));
		cdb[0] = SCSI_WRITE_10;
		cdb[2] = (lba >> 24) & 0xFF;
		cdb[3] = (lba >> 16) & 0xFF;
		cdb[4] = (lba >> 8) & 0xFF;
		cdb[5] = lba & 0xFF;
		cdb[7] = 0;
		cdb[8] = 1;

		if (!usb_storage_scsi_command(data, cdb, 10, (uint8_t*)buffer + i * data->block_size, data->block_size, false)) {
			debugf(ERROR, "usb_storage: write failed at sector %d", lba);
			return;
		}
	}
}

void usb_storage_init(driver_t* driver) {
	usb_storage_data_t* data = (usb_storage_data_t*)driver->driver_specific_data;

	debugf(INFO, "usb_storage: initializing (addr=%d, ep_in=0x%x, ep_out=0x%x)", data->dev->address, data->ep_in.bEndpointAddress, data->ep_out.bEndpointAddress);

	for (int i = 0; i < 5; i++) {
		if (usb_storage_test_unit_ready(data)) {
			break;
		}
	}

	if (!usb_storage_inquiry(data)) {
		debugf(WARNING, "usb_storage: INQUIRY failed");
		return;
	}

	if (!usb_storage_read_capacity(data)) {
		debugf(WARNING, "usb_storage: READ CAPACITY failed");
		return;
	}

	if (!read_gpt((disk_driver_t*)driver)) {
		debugf(WARNING, "usb_storage: no GPT found");
	}

	register_disk((disk_driver_t*)driver);
}

disk_driver_t* usb_storage_create(usb_hc_driver_t* hc, usb_device_t* dev, usb_endpoint_descriptor_t* ep_in, usb_endpoint_descriptor_t* ep_out) {
	disk_driver_t* driver = (disk_driver_t*)kmalloc(sizeof(disk_driver_t) + sizeof(usb_storage_data_t));
	memset(driver, 0, sizeof(disk_driver_t) + sizeof(usb_storage_data_t));

	driver->driver.is_device_present = usb_storage_is_device_present;
	driver->driver.get_device_name = usb_storage_get_device_name;
	driver->driver.init = usb_storage_init;

	driver->flush = usb_storage_flush;
	driver->read = usb_storage_read;
	driver->write = usb_storage_write;
	driver->physical = true;

	usb_storage_data_t* data = (usb_storage_data_t*)&driver[1];
	driver->driver.driver_specific_data = data;

	data->hc = hc;
	data->dev = dev;
	memcpy(&data->ep_in, ep_in, sizeof(usb_endpoint_descriptor_t));
	memcpy(&data->ep_out, ep_out, sizeof(usb_endpoint_descriptor_t));
	data->toggle_in = 0;
	data->toggle_out = 0;
	data->tag = 1;
	data->lun = 0;
	data->block_size = 512;

	return driver;
}
