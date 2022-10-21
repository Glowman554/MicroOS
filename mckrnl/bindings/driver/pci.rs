#[repr(C)]
#[derive(Debug)]
pub struct PciDeviceHeader {
	pub vendor_id: u16,
	pub device_id: u16,
	pub revision_id: u8,
	pub prog_if: u8,
	pub subclass: u8,
	pub class: u8,
	pub bar0: u32,
	pub bar1: u32,
	pub bar2: u32,
	pub bar3: u32,
	pub bar4: u32,
	pub bar5: u32,
	pub interrupt_line: u8,
	pub interrupt_pin: u8
}

type PciLoadDriver = extern fn(header: PciDeviceHeader, bus: u16, device: u16, function: u16);

extern {
	pub fn register_pci_driver_cs(class: u8, subclass: u8, prog_IF: u8, load: PciLoadDriver);
	pub fn register_pci_driver_vd(vendor_id: u16, device_id: u16, load: PciLoadDriver);

	pub fn enable_mmio(bus: u16, device: u16, function: u16);
	pub fn become_bus_master(bus: u16, device: u16, function: u16);

	pub fn pci_readd(bus: u16, device: u16, function: u16, registeroffset: u32) -> u32;
	pub fn pci_writed(bus: u16, device: u16, function: u16, registeroffset: u32, value: u32);

	pub fn pci_readw(bus: u16, device: u16, function: u16, registeroffset: u32) -> u16;
	pub fn pci_writew(bus: u16, device: u16, function: u16, registeroffset: u32, value: u16);

	pub fn pci_readb(bus: u16, device: u16, function: u16, registeroffset: u32) -> u8;
	pub fn pci_writeb(bus: u16, device: u16, function: u16, registeroffset: u32, value: u8);
}