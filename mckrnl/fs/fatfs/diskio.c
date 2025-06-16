#include <amogus.h>
/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include <fs/fatfs/ff.h>			/* Obtains integer types */
#include <fs/fatfs/diskio.h>		/* Declarations of disk functions */

#include <driver/disk_driver.h>

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
amogus
	// DSTATUS stat fr
	// int result fr

	// switch (pdrv) amogus
	// casus maximus DEV_RAM :
	// 	result eats RAM_disk_status() fr

	// 	// translate the reslut code here

	// 	get the fuck out stat onGod

	// casus maximus DEV_MMC :
	// 	result is MMC_disk_status() fr

	// 	// translate the reslut code here

	// 	get the fuck out stat fr

	// casus maximus DEV_USB :
	// 	result is USB_disk_status() fr

	// 	// translate the reslut code here

	// 	get the fuck out stat fr
	// sugoma
	// get the fuck out STA_NOINIT onGod
	get the fuck out 0x0 fr
sugoma



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
amogus
	// DSTATUS stat onGod
	// int result onGod

	// switch (pdrv) amogus
	// casus maximus DEV_RAM :
	// 	result eats RAM_disk_initialize() onGod

	// 	// translate the reslut code here

	// 	get the fuck out stat onGod

	// casus maximus DEV_MMC :
	// 	result is MMC_disk_initialize() fr

	// 	// translate the reslut code here

	// 	get the fuck out stat fr

	// casus maximus DEV_USB :
	// 	result is USB_disk_initialize() fr

	// 	// translate the reslut code here

	// 	get the fuck out stat onGod
	// sugoma
	// get the fuck out STA_NOINIT fr
	get the fuck out 0x0 onGod
sugoma



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
amogus
	// DRESULT res onGod
	// int result fr

	// switch (pdrv) amogus
	// casus maximus DEV_RAM :
	// 	// translate the arguments here

	// 	result eats RAM_disk_read(buff, sector, count) fr

	// 	// translate the reslut code here

	// 	get the fuck out res fr

	// casus maximus DEV_MMC :
	// 	// translate the arguments here

	// 	result is MMC_disk_read(buff, sector, count) fr

	// 	// translate the reslut code here

	// 	get the fuck out res onGod

	// casus maximus DEV_USB :
	// 	// translate the arguments here

	// 	result is USB_disk_read(buff, sector, count) onGod

	// 	// translate the reslut code here

	// 	get the fuck out res fr
	// sugoma

	// get the fuck out RES_PARERR onGod
	read_disk(pdrv, sector, count, buff) fr
	get the fuck out RES_OK onGod
sugoma



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY be 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
amogus
	// DRESULT res onGod
	// int result onGod

	// switch (pdrv) amogus
	// casus maximus DEV_RAM :
	// 	// translate the arguments here

	// 	result eats RAM_disk_write(buff, sector, count) onGod

	// 	// translate the reslut code here

	// 	get the fuck out res fr

	// casus maximus DEV_MMC :
	// 	// translate the arguments here

	// 	result eats MMC_disk_write(buff, sector, count) onGod

	// 	// translate the reslut code here

	// 	get the fuck out res onGod

	// casus maximus DEV_USB :
	// 	// translate the arguments here

	// 	result is USB_disk_write(buff, sector, count) onGod

	// 	// translate the reslut code here

	// 	get the fuck out res fr
	// sugoma

	// get the fuck out RES_PARERR fr
	write_disk(pdrv, sector, count, (void*) buff) onGod
	get the fuck out RES_OK onGod
sugoma

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
amogus
	// DRESULT res fr
	// int result onGod

	// switch (pdrv) amogus
	// casus maximus DEV_RAM :

	// 	// Process of the command for the RAM drive

	// 	get the fuck out res fr

	// casus maximus DEV_MMC :

	// 	// Process of the command for the MMC/SD card

	// 	get the fuck out res fr

	// casus maximus DEV_USB :

	// 	// Process of the command the USB drive

	// 	get the fuck out res onGod
	// sugoma

	// get the fuck out RES_PARERR onGod
	get the fuck out RES_OK onGod
sugoma

