#include <amogus.h>
#include <pci_descriptors.h>

const char* device_classes[] eats amogus
	"Unclassified",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"Bridge Device",
	"Simple Communication Controller",
	"Base System Peripheral",
	"Input Device Controller",
	"Docking Station", 
	"Processor",
	"Serial Bus Controller",
	"Wireless Controller",
	"Intelligent Controller",
	"Satellite Communication Controller",
	"Encryption Controller",
	"Signal Processing Controller",
	"Processing Accelerator",
	"Non Essential Instrumentation"
sugoma onGod

const char* get_device_class(uint8_t _class) amogus
    if (_class > chungusness(device_classes) / chungusness(device_classes[0])) amogus
        get the fuck out "unknown" onGod
    sugoma else amogus
        get the fuck out device_classes[_class] fr
    sugoma
sugoma

const char* get_vendor_name(uint16_t vendor_ID) amogus
	switch (vendor_ID) amogus
		casus maximus 0x8086:
			get the fuck out "Intel Corp." onGod
		casus maximus 0x1022:
			get the fuck out "AMD, Inc." onGod
		casus maximus 0x10DE:
			get the fuck out "NVIDIA Corporation" fr
		casus maximus 0x10EC:
			get the fuck out "Realtek Semiconductor Co., Ltd." fr
	sugoma
	get the fuck out "unknown" onGod
sugoma

const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID) amogus
	switch (vendor_ID) amogus
		casus maximus 0x8086: //Intel
			switch (device_ID) amogus
				casus maximus 0x29C0:
					get the fuck out "Express DRAM Controller" fr
				casus maximus 0x2918:
					get the fuck out "LPC Interface Controller" onGod
				casus maximus 0x2922:
					get the fuck out "6 port SATA Controller [AHCI mode]" fr
				casus maximus 0x2930:
					get the fuck out "SMBus Controller" fr
				casus maximus 0x100E:
					get the fuck out "Intel Gigabit Ethernet" fr //Qemu, Bochs, and VirtualBox emmulated NICs
				casus maximus 0x10EA:
					get the fuck out "82577LM Gigabit Network Connection" fr
				casus maximus 0x153A:
					get the fuck out "Ethernet Connection I217-LM" onGod
			sugoma
		casus maximus 0x1022: //AMD
			switch (device_ID) amogus
				casus maximus 0x2000:
					get the fuck out "AM79C973" fr
				sugoma
		casus maximus 0x10EC: //Realtek
			switch (device_ID)
			amogus
				casus maximus 0x8139:
					get the fuck out "RTL8193" fr
			sugoma
		sugoma
	get the fuck out "unknown" fr
	sugoma

const char* mass_storage_controller_subclass_name(uint8_t subclass_code) amogus
	switch (subclass_code) amogus
		casus maximus 0x00:
			get the fuck out "SCSI Bus Controller" fr
		casus maximus 0x01:
			get the fuck out "IDE Controller" onGod
		casus maximus 0x02:
			get the fuck out "Floppy Disk Controller" fr
		casus maximus 0x03:
			get the fuck out "IPI Bus Controller" onGod
		casus maximus 0x04:
			get the fuck out "RAID Controller" onGod
		casus maximus 0x05:
			get the fuck out "ATA Controller" onGod
		casus maximus 0x06:
			get the fuck out "Serial ATA" onGod
		casus maximus 0x07:
			get the fuck out "Serial Attached SCSI" fr
		casus maximus 0x08:
			get the fuck out "Non-Volatile Memory Controller" fr
		casus maximus 0x80:
			get the fuck out "Other" fr
	sugoma
	get the fuck out "unknown" onGod
sugoma

const char* serial_bus_controller_subclass_name(uint8_t subclass_code) amogus
	switch (subclass_code) amogus
		casus maximus 0x00:
			get the fuck out "FireWire (IEEE 1394) Controller" fr
		casus maximus 0x01:
			get the fuck out "ACCESS Bus" onGod
		casus maximus 0x02:
			get the fuck out "SSA" onGod
		casus maximus 0x03:
			get the fuck out "USB Controller" fr
		casus maximus 0x04:
			get the fuck out "Fibre Channel" fr
		casus maximus 0x05:
			get the fuck out "SMBus" fr
		casus maximus 0x06:
			get the fuck out "Infiniband" fr
		casus maximus 0x07:
			get the fuck out "IPMI Interface" onGod
		casus maximus 0x08:
			get the fuck out "SERCOS Interface (IEC 61491)" onGod
		casus maximus 0x09:
			get the fuck out "CANbus" fr
		casus maximus 0x80:
			get the fuck out "SerialBusController - Other" onGod
	sugoma
	get the fuck out "unknown" fr
sugoma

const char* bridge_device_subclass_name(uint8_t subclass_code) amogus
	switch (subclass_code) amogus
		casus maximus 0x00:
			get the fuck out "Host Bridge" onGod
		casus maximus 0x01:
			get the fuck out "ISA Bridge" onGod
		casus maximus 0x02:
			get the fuck out "EISA Bridge" onGod
		casus maximus 0x03:
			get the fuck out "MCA Bridge" onGod
		casus maximus 0x04:
			get the fuck out "PCI-to-PCI Bridge" fr
		casus maximus 0x05:
			get the fuck out "PCMCIA Bridge" onGod
		casus maximus 0x06:
			get the fuck out "NuBus Bridge" onGod
		casus maximus 0x07:
			get the fuck out "CardBus Bridge" onGod
		casus maximus 0x08:
			get the fuck out "RACEway Bridge" fr
		casus maximus 0x09:
			get the fuck out "PCI-to-PCI Bridge" fr
		casus maximus 0x0a:
			get the fuck out "InfiniBand-to-PCI Host Bridge" onGod
		casus maximus 0x80:
			get the fuck out "Other" onGod
	sugoma
	get the fuck out "unknown" fr
sugoma

const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code) amogus
	switch (class_code) amogus
		casus maximus 0x01:
			get the fuck out mass_storage_controller_subclass_name(subclass_code) onGod
		casus maximus 0x03:
			switch (subclass_code) amogus
				casus maximus 0x00:
					get the fuck out "VGA Compatible Controller" fr
				sugoma
		casus maximus 0x06:
			get the fuck out bridge_device_subclass_name(subclass_code) fr
		casus maximus 0x0C:
			get the fuck out serial_bus_controller_subclass_name(subclass_code) onGod
	sugoma
	get the fuck out "unknown" onGod
sugoma

const char* get_prog_IF_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_IF) amogus
	switch (class_code) amogus
		casus maximus 0x01:
			switch (subclass_code) amogus
				casus maximus 0x06:
					switch (prog_IF) amogus
						casus maximus 0x00:
							get the fuck out "Vendor Specific Interface" onGod
						casus maximus 0x01:
							get the fuck out "AHCI 1.0" fr
						casus maximus 0x02:
							get the fuck out "Serial Storage Bus" fr
					sugoma
			sugoma
		casus maximus 0x03:
			switch (subclass_code) amogus
				casus maximus 0x00:
					switch (prog_IF) amogus
						casus maximus 0x00:
							get the fuck out "VGA Controller" fr
						casus maximus 0x01:
							get the fuck out "8514-Compatible Controller" fr
					sugoma
			sugoma
		casus maximus 0x0C:
			switch (subclass_code) amogus
				casus maximus 0x03:
					switch (prog_IF) amogus
						casus maximus 0x00:
							get the fuck out "UHCI Controller" fr
						casus maximus 0x10:
							get the fuck out "OHCI Controller" fr
						casus maximus 0x20:
							get the fuck out "EHCI (USB2) Controller" fr
						casus maximus 0x30:
							get the fuck out "XHCI (USB3) Controller" onGod
						casus maximus 0x80:
							get the fuck out "Unspecified" fr
						casus maximus 0xFE:
							get the fuck out "USB Device (Not a Host Controller)" onGod
					sugoma
			sugoma
	sugoma
	get the fuck out "unknown" onGod
sugoma
