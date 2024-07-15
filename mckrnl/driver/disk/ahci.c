#include <driver/disk/ahci.h>

#include <driver/disk_driver.h>
#include <driver/driver.h>
#include <fs/gpt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <memory/vmm.h>

void ahci_port_stop_command(ahci_driver_data_t* data) {
	data->hba_port->cmd_sts &= ~HBA_PxCMD_ST;
	data->hba_port->cmd_sts &= ~HBA_PxCMD_FRE;

	while (true) {
		if (data->hba_port->cmd_sts & HBA_PxCMD_FR || data->hba_port->cmd_sts & HBA_PxCMD_CR) {
            continue;
        }
		break;
	}
}

void ahci_port_start_command(ahci_driver_data_t* data) {
	while (data->hba_port->cmd_sts & HBA_PxCMD_CR) {
		__asm__ __volatile__("pause");
	}

	data->hba_port->cmd_sts |= HBA_PxCMD_FRE;
	data->hba_port->cmd_sts |= HBA_PxCMD_ST;
}


bool ahci_port_is_device_present(driver_t* driver) {
    return true;
}

char* ahci_port_get_device_name(driver_t* driver) {
    ahci_driver_data_t* data = (ahci_driver_data_t*) driver->driver_specific_data;
    return data->name;
}

void ahci_port_init(driver_t* driver) {
    ahci_driver_data_t* data = (ahci_driver_data_t*) driver->driver_specific_data;
    data->buffer = vmm_alloc(1);
	memset(data->buffer, 0, 0x1000);

    ahci_port_stop_command(data);

	void* new_base = vmm_alloc(1);
	data->hba_port->command_list_base = (uint32_t) new_base;
	data->hba_port->command_list_base_upper = 0;
	memset(new_base, 0, 1024);

	void* fis_base = vmm_alloc(1);
	data->hba_port->fis_base_address = (uint32_t) fis_base;
	data->hba_port->fis_base_address_upper = 0;
	memset(fis_base, 0, 256);

	HBA_command_header_t* cmd_header = (HBA_command_header_t*) data->hba_port->command_list_base;

	for (int i = 0; i < 32; i++){
		cmd_header[i].prdt_length = 8;

		void* cmd_table_address = vmm_alloc(1);
		uint64_t address = (uint64_t)cmd_table_address + (i << 8);
		cmd_header[i].command_table_base_address = (uint32_t) address;
		cmd_header[i].command_table_base_address_upper = (uint32_t) (address >> 32);
		memset(cmd_table_address, 0, 256);
	}

    ahci_port_start_command(data);

    if (!read_gpt((disk_driver_t*) driver)) {
		debugf("AHCI: Failed to read GPT");
        breakpoint();
	}

	register_disk((disk_driver_t*) driver);
}

void ahci_port_flush(disk_driver_t* driver) {
}

void ahci_port_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
    ahci_driver_data_t* data = (ahci_driver_data_t*) driver->driver.driver_specific_data;
    vmm_context_t context = vmm_get_current_context();
    void* physical_buffer = vmm_lookup((uintptr_t) buffer, &context);
	if (physical_buffer == NULL) {
		debugf("AHCI: Failed to get physical address of buffer!");
        breakpoint();
		return;
	}

	uint64_t spin = 0;
	while ((data->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin ++;
	}
	if (spin == 1000000) {
		debugf("AHCI: Timeout while waiting for device to be ready!");
		return;
	}

	uint32_t sector_low = (uint32_t) sector;
	uint32_t sector_high = (uint32_t) (sector >> 32);

	data->hba_port->interrupt_status = (uint32_t)-1; // Clear pending interrupt bits

	HBA_command_header_t* cmd_header = (HBA_command_header_t*) data->hba_port->command_list_base;
	cmd_header->command_fis_length = sizeof(FIS_REG_H2D_t) / sizeof(uint32_t); //command FIS size;
	cmd_header->write = 0; // Indicate a read
	cmd_header->prdt_length = 1;

	HBA_command_table_t* command_table = (HBA_command_table_t*) cmd_header->command_table_base_address;
	memset(command_table, 0, sizeof(HBA_command_table_t) + (cmd_header->prdt_length-1) * sizeof(HBA_PRDT_entry_t));

	command_table->prdt_entry[0].data_base_address = (uint32_t) physical_buffer;
	command_table->prdt_entry[0].data_base_address_upper = 0;
	command_table->prdt_entry[0].byte_count = (count << 9) - 1; // 512 bytes per sector
	command_table->prdt_entry[0].interrupt_on_completion = 1;

	FIS_REG_H2D_t* cmd_fis = (FIS_REG_H2D_t*) &command_table->command_fis;

	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->command_control = 1; // Command
	cmd_fis->command = ATA_CMD_READ_DMA_EX;

	cmd_fis->lba0 = (uint8_t)sector_low;
	cmd_fis->lba1 = (uint8_t)(sector_low >> 8);
	cmd_fis->lba2 = (uint8_t)(sector_low >> 16);
	cmd_fis->lba3 = (uint8_t)sector_high;
	cmd_fis->lba4 = (uint8_t)(sector_high >> 8);
	cmd_fis->lba5 = (uint8_t)(sector_high >> 16);

	cmd_fis->device_register = 1 << 6; //LBA mode

	cmd_fis->count_low = count & 0xFF;
	cmd_fis->count_high = (count >> 8) & 0xFF;

	data->hba_port->command_issue = 1;

	while (true) {
		if (data->hba_port->command_issue == 0) {
            break;
        }
		if (data->hba_port->interrupt_status & HBA_PxIS_TFES) {
			return;
		}
	}
}

void ahci_port_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) {
    ahci_driver_data_t* data = (ahci_driver_data_t*) driver->driver.driver_specific_data;
    vmm_context_t context = vmm_get_current_context();
    void* physical_buffer = vmm_lookup((uintptr_t) buffer, &context);
	if (physical_buffer == NULL) {
		debugf("AHCI: Failed to get physical address of buffer!");
        breakpoint();
		return;
	}

	uint64_t spin = 0;
	while ((data->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin ++;
	}
	if (spin == 1000000) {
		debugf("AHCI: Timeout while waiting for device to be ready!");
		return;
	}

	uint32_t sector_low = (uint32_t) sector;
	uint32_t sector_high = (uint32_t) (sector >> 32);

	data->hba_port->interrupt_status = (uint32_t)-1; // Clear pending interrupt bits

	HBA_command_header_t* cmd_header = (HBA_command_header_t*) data->hba_port->command_list_base;
	cmd_header->command_fis_length = sizeof(FIS_REG_H2D_t) / sizeof(uint32_t); //command FIS size;
	cmd_header->write = 1; // Indicate a write
	cmd_header->prdt_length = 1;

    HBA_command_table_t* commandTable = (HBA_command_table_t*) cmd_header->command_table_base_address;
	memset(commandTable, 0, sizeof(HBA_command_table_t) + (cmd_header->prdt_length - 1) * sizeof(HBA_PRDT_entry_t));

	commandTable->prdt_entry[0].data_base_address = (uint32_t)(uint64_t) physical_buffer;
	commandTable->prdt_entry[0].data_base_address_upper = (uint32_t)((uint64_t) physical_buffer >> 32);
	commandTable->prdt_entry[0].byte_count = (count << 9) - 1; // 512 bytes per sector
	commandTable->prdt_entry[0].interrupt_on_completion = 1;

	FIS_REG_H2D_t* cmd_fis = (FIS_REG_H2D_t*) &commandTable->command_fis;

	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->command_control = 1; // Command
	cmd_fis->command = ATA_CMD_WRITE_DMA_EX;

	cmd_fis->lba0 = (uint8_t)sector_low;
	cmd_fis->lba1 = (uint8_t)(sector_low >> 8);
	cmd_fis->lba2 = (uint8_t)(sector_low >> 16);
	cmd_fis->lba3 = (uint8_t)sector_high;
	cmd_fis->lba4 = (uint8_t)(sector_high >> 8);
	cmd_fis->lba5 = (uint8_t)(sector_high >> 16);

	cmd_fis->device_register = 1 << 6; //LBA mode

	cmd_fis->count_low = count & 0xFF;
	cmd_fis->count_high = (count >> 8) & 0xFF;

	data->hba_port->command_issue = 1;

	while (true) {
		if (data->hba_port->command_issue == 0) {
            break;
        }
		if (data->hba_port->interrupt_status & HBA_PxIS_TFES) {
			return;
		}
	}
}


disk_driver_t* get_ahci_driver(HBA_port_t* hba_port, port_type_t port_type, uint8_t port_number) {
    disk_driver_t* driver = (disk_driver_t*) vmm_alloc(1);
	memset(driver, 0, 0x1000);

	driver->driver.is_device_present = ahci_port_is_device_present;
	driver->driver.get_device_name = ahci_port_get_device_name;
	driver->driver.init = ahci_port_init;
	
	driver->flush = ahci_port_flush;
	driver->read = ahci_port_read;
	driver->write = ahci_port_write;

	driver->physical = true;

	driver->driver.driver_specific_data = &driver[1];
	ahci_driver_data_t* data = (ahci_driver_data_t*) driver->driver.driver_specific_data;
	
    // TODO
	strcpy(data->name, "ahci");
    data->hba_port = hba_port;
    data->port_type = port_type;
    data->port_number = port_number;

    return driver;
}


port_type_t ahci_check_port_type(HBA_port_t* port) {
	uint32_t sata_status = port->sata_status;

	uint8_t interface_power_management = (sata_status >> 8) & 0b111;
	uint8_t device_detection = sata_status & 0b111;

	if (device_detection != HBA_PORT_DEV_PRESENT || interface_power_management != HBA_PORT_IPM_ACTIVE) {
        return None;
    }

	switch (port->signature){
		case SATA_SIG_ATAPI:
			return SATAPI;
		case SATA_SIG_ATA:
			return SATA;
		case SATA_SIG_PM:
			return PM;
		case SATA_SIG_SEMB:
			return SEMB;
		default:
			return None;
	}
}


void ahci_probe_ports(HBA_memory_t* ABAR) {
    uint32_t ports_implemented = ABAR->ports_implemented;
    int port_count = 0;
	for (int i = 0; i < 32; i++){
		if (ports_implemented & (1 << i)) {
			port_type_t port_type = ahci_check_port_type(&ABAR->ports[i]);

			if (port_type == SATA || port_type == SATAPI) {
				debugf("Found SATA/SATAPI port %d", i);
                register_driver((driver_t*) get_ahci_driver(&ABAR->ports[i], port_type, port_count));
				port_count++;
			}
		}
	}
}

void ahci_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) {
    HBA_memory_t* ABAR = (HBA_memory_t*) header.BAR5;
    vmm_map_page(kernel_context, (uintptr_t) ABAR, (uintptr_t) ABAR, PTE_PRESENT | PTE_WRITE);

    ahci_probe_ports(ABAR);
}