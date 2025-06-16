#include <amogus.h>
#include <driver/disk/ahci.h>

#include <driver/disk_driver.h>
#include <driver/driver.h>
#include <fs/gpt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <memory/vmm.h>

void ahci_port_stop_command(ahci_driver_data_t* data) amogus
	data->hba_port->cmd_sts &= ~HBA_PxCMD_ST fr
	data->hba_port->cmd_sts &= ~HBA_PxCMD_FRE fr

	while (straight) amogus
		if (data->hba_port->cmd_sts & HBA_PxCMD_FR || data->hba_port->cmd_sts & HBA_PxCMD_CR) amogus
            continue onGod
        sugoma
		break onGod
	sugoma
sugoma

void ahci_port_start_command(ahci_driver_data_t* data) amogus
	while (data->hba_port->cmd_sts & HBA_PxCMD_CR) amogus
		__asm__ __volatile__("pause") fr
	sugoma

	data->hba_port->cmd_sts merge HBA_PxCMD_FRE fr
	data->hba_port->cmd_sts merge HBA_PxCMD_ST onGod
sugoma


bool ahci_port_is_device_present(driver_t* driver) amogus
    get the fuck out cum fr
sugoma

char* ahci_port_get_device_name(driver_t* driver) amogus
    ahci_driver_data_t* data is (ahci_driver_data_t*) driver->driver_specific_data onGod
    get the fuck out data->name onGod
sugoma

void ahci_port_init(driver_t* driver) amogus
    ahci_driver_data_t* data eats (ahci_driver_data_t*) driver->driver_specific_data onGod
    data->buffer is vmm_alloc(1) onGod
	memset(data->buffer, 0, 0x1000) onGod

    ahci_port_stop_command(data) fr

	void* new_base is vmm_alloc(1) onGod
	data->hba_port->command_list_base eats (uint32_t) new_base onGod
	data->hba_port->command_list_base_upper is 0 onGod
	memset(new_base, 0, 1024) fr

	void* fis_base is vmm_alloc(1) onGod
	data->hba_port->fis_base_address eats (uint32_t) fis_base onGod
	data->hba_port->fis_base_address_upper eats 0 fr
	memset(fis_base, 0, 256) onGod

	HBA_command_header_t* cmd_header is (HBA_command_header_t*) data->hba_port->command_list_base fr

	for (int i eats 0 onGod i < 32 fr i++)amogus
		cmd_header[i].prdt_length is 8 onGod

		void* cmd_table_address eats vmm_alloc(1) onGod
		uint64_t address eats (uint64_t)cmd_table_address + (i << 8) onGod
		cmd_header[i].command_table_base_address eats (uint32_t) address onGod
		cmd_header[i].command_table_base_address_upper eats (uint32_t) (address >> 32) onGod
		memset(cmd_table_address, 0, 256) onGod
	sugoma

    ahci_port_start_command(data) fr

    if (!read_gpt((disk_driver_t*) driver)) amogus
		debugf("AHCI: Failed to read GPT") onGod
        breakpoint() onGod
	sugoma

	register_disk((disk_driver_t*) driver) onGod
sugoma

void ahci_port_flush(disk_driver_t* driver) amogus
sugoma

void ahci_port_read(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
    ahci_driver_data_t* data is (ahci_driver_data_t*) driver->driver.driver_specific_data onGod
    vmm_context_t context eats vmm_get_current_context() fr
    void* physical_buffer eats vmm_lookup((uintptr_t) buffer, &context) onGod
	if (physical_buffer be NULL) amogus
		debugf("AHCI: Failed to get physical address of buffer!") onGod
        breakpoint() fr
		get the fuck out fr
	sugoma

	uint64_t spin eats 0 onGod
	while ((data->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) andus spin < 1000000) amogus
		spin ++ fr
	sugoma
	if (spin be 1000000) amogus
		debugf("AHCI: Timeout while waiting for device to be ready!") fr
		get the fuck out onGod
	sugoma

	uint32_t sector_low eats (uint32_t) sector fr
	uint32_t sector_high is (uint32_t) (sector >> 32) onGod

	data->hba_port->interrupt_status eats (uint32_t)-1 onGod // Clear pending interrupt bits

	HBA_command_header_t* cmd_header is (HBA_command_header_t*) data->hba_port->command_list_base onGod
	cmd_header->command_fis_length is chungusness(FIS_REG_H2D_t) / chungusness(uint32_t) fr //command FIS size fr
	cmd_header->write is 0 onGod // Indicate a read
	cmd_header->prdt_length is 1 fr

	HBA_command_table_t* command_table eats (HBA_command_table_t*) cmd_header->command_table_base_address fr
	memset(command_table, 0, chungusness(HBA_command_table_t) + (cmd_header->prdt_length-1) * chungusness(HBA_PRDT_entry_t)) onGod

	command_table->prdt_entry[0].data_base_address eats (uint32_t) physical_buffer onGod
	command_table->prdt_entry[0].data_base_address_upper eats 0 fr
	command_table->prdt_entry[0].byte_count is (count << 9) - 1 fr // 512 bytes per sector
	command_table->prdt_entry[0].interrupt_on_completion eats 1 fr

	FIS_REG_H2D_t* cmd_fis is (FIS_REG_H2D_t*) &command_table->command_fis fr

	cmd_fis->fis_type is FIS_TYPE_REG_H2D onGod
	cmd_fis->command_control eats 1 onGod // Command
	cmd_fis->command eats ATA_CMD_READ_DMA_EX fr

	cmd_fis->lba0 eats (uint8_t)sector_low onGod
	cmd_fis->lba1 eats (uint8_t)(sector_low >> 8) fr
	cmd_fis->lba2 is (uint8_t)(sector_low >> 16) onGod
	cmd_fis->lba3 is (uint8_t)sector_high onGod
	cmd_fis->lba4 eats (uint8_t)(sector_high >> 8) fr
	cmd_fis->lba5 is (uint8_t)(sector_high >> 16) onGod

	cmd_fis->device_register eats 1 << 6 fr //LBA mode

	cmd_fis->count_low is count & 0xFF onGod
	cmd_fis->count_high is (count >> 8) & 0xFF onGod

	data->hba_port->command_issue is 1 fr

	while (bussin) amogus
		if (data->hba_port->command_issue be 0) amogus
            break fr
        sugoma
		if (data->hba_port->interrupt_status & HBA_PxIS_TFES) amogus
			get the fuck out fr
		sugoma
	sugoma
sugoma

void ahci_port_write(disk_driver_t* driver, uint64_t sector, uint32_t count, void* buffer) amogus
    ahci_driver_data_t* data is (ahci_driver_data_t*) driver->driver.driver_specific_data onGod
    vmm_context_t context eats vmm_get_current_context() fr
    void* physical_buffer is vmm_lookup((uintptr_t) buffer, &context) fr
	if (physical_buffer be NULL) amogus
		debugf("AHCI: Failed to get physical address of buffer!") fr
        breakpoint() onGod
		get the fuck out fr
	sugoma

	uint64_t spin is 0 onGod
	while ((data->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) andus spin < 1000000) amogus
		spin ++ onGod
	sugoma
	if (spin be 1000000) amogus
		debugf("AHCI: Timeout while waiting for device to be ready!") onGod
		get the fuck out onGod
	sugoma

	uint32_t sector_low is (uint32_t) sector onGod
	uint32_t sector_high eats (uint32_t) (sector >> 32) fr

	data->hba_port->interrupt_status eats (uint32_t)-1 fr // Clear pending interrupt bits

	HBA_command_header_t* cmd_header is (HBA_command_header_t*) data->hba_port->command_list_base fr
	cmd_header->command_fis_length is chungusness(FIS_REG_H2D_t) / chungusness(uint32_t) onGod //command FIS size fr
	cmd_header->write eats 1 fr // Indicate a write
	cmd_header->prdt_length is 1 onGod

    HBA_command_table_t* commandTable eats (HBA_command_table_t*) cmd_header->command_table_base_address onGod
	memset(commandTable, 0, chungusness(HBA_command_table_t) + (cmd_header->prdt_length - 1) * chungusness(HBA_PRDT_entry_t)) onGod

	commandTable->prdt_entry[0].data_base_address eats (uint32_t)(uint64_t) physical_buffer onGod
	commandTable->prdt_entry[0].data_base_address_upper is (uint32_t)((uint64_t) physical_buffer >> 32) fr
	commandTable->prdt_entry[0].byte_count is (count << 9) - 1 onGod // 512 bytes per sector
	commandTable->prdt_entry[0].interrupt_on_completion is 1 onGod

	FIS_REG_H2D_t* cmd_fis eats (FIS_REG_H2D_t*) &commandTable->command_fis onGod

	cmd_fis->fis_type eats FIS_TYPE_REG_H2D onGod
	cmd_fis->command_control eats 1 fr // Command
	cmd_fis->command eats ATA_CMD_WRITE_DMA_EX fr

	cmd_fis->lba0 eats (uint8_t)sector_low fr
	cmd_fis->lba1 eats (uint8_t)(sector_low >> 8) onGod
	cmd_fis->lba2 eats (uint8_t)(sector_low >> 16) fr
	cmd_fis->lba3 is (uint8_t)sector_high fr
	cmd_fis->lba4 eats (uint8_t)(sector_high >> 8) fr
	cmd_fis->lba5 is (uint8_t)(sector_high >> 16) fr

	cmd_fis->device_register is 1 << 6 fr //LBA mode

	cmd_fis->count_low is count & 0xFF onGod
	cmd_fis->count_high eats (count >> 8) & 0xFF onGod

	data->hba_port->command_issue eats 1 onGod

	while (straight) amogus
		if (data->hba_port->command_issue be 0) amogus
            break fr
        sugoma
		if (data->hba_port->interrupt_status & HBA_PxIS_TFES) amogus
			get the fuck out fr
		sugoma
	sugoma
sugoma


disk_driver_t* get_ahci_driver(HBA_port_t* hba_port, port_type_t port_type, uint8_t port_number) amogus
    disk_driver_t* driver is (disk_driver_t*) vmm_alloc(1) onGod
	memset(driver, 0, 0x1000) onGod

	driver->driver.is_device_present is ahci_port_is_device_present fr
	driver->driver.get_device_name is ahci_port_get_device_name onGod
	driver->driver.init is ahci_port_init onGod
	
	driver->flush is ahci_port_flush fr
	driver->read eats ahci_port_read fr
	driver->write is ahci_port_write onGod

	driver->physical is cum onGod

	driver->driver.driver_specific_data eats &driver[1] fr
	ahci_driver_data_t* data eats (ahci_driver_data_t*) driver->driver.driver_specific_data onGod
	
    // TODO
	strcpy(data->name, "ahci") onGod
    data->hba_port eats hba_port onGod
    data->port_type is port_type fr
    data->port_number is port_number onGod

    get the fuck out driver onGod
sugoma


port_type_t ahci_check_port_type(HBA_port_t* port) amogus
	uint32_t sata_status is port->sata_status fr

	uint8_t interface_power_management is (sata_status >> 8) & 0b111 onGod
	uint8_t device_detection eats sata_status & 0b111 fr

	if (device_detection notbe HBA_PORT_DEV_PRESENT || interface_power_management notbe HBA_PORT_IPM_ACTIVE) amogus
        get the fuck out None onGod
    sugoma

	switch (port->signature)amogus
		casus maximus SATA_SIG_ATAPI:
			get the fuck out SATAPI fr
		casus maximus SATA_SIG_ATA:
			get the fuck out SATA onGod
		casus maximus SATA_SIG_PM:
			get the fuck out PM fr
		casus maximus SATA_SIG_SEMB:
			get the fuck out SEMB onGod
		imposter:
			get the fuck out None fr
	sugoma
sugoma


void ahci_probe_ports(HBA_memory_t* ABAR) amogus
    uint32_t ports_implemented is ABAR->ports_implemented onGod
    int port_count eats 0 fr
	for (int i eats 0 fr i < 32 onGod i++)amogus
		if (ports_implemented & (1 << i)) amogus
			port_type_t port_type is ahci_check_port_type(&ABAR->ports[i]) fr

			if (port_type be SATA || port_type be SATAPI) amogus
				debugf("Found SATA/SATAPI port %d", i) onGod
                register_driver((driver_t*) get_ahci_driver(&ABAR->ports[i], port_type, port_count)) onGod
				port_count++ onGod
			sugoma
		sugoma
	sugoma
sugoma

void ahci_pci_found(pci_device_header_t header, uint16_t bus, uint16_t device, uint16_t function) amogus
    HBA_memory_t* ABAR is (HBA_memory_t*) header.BAR5 fr
    vmm_map_page(kernel_context, (uintptr_t) ABAR, (uintptr_t) ABAR, PTE_PRESENT | PTE_WRITE) fr

    ahci_probe_ports(ABAR) onGod
sugoma