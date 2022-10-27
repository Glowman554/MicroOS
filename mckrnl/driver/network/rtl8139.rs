extern crate alloc;
use alloc::vec::Vec;

use core::ffi::{c_char, c_void};
use cstr_core::CString;
use lazy_static::lazy_static;
use crate::{bindings::{driver::{pci::{PciDeviceHeader, become_bus_master}, Driver, nic_driver::{NicDriver, Ip, Mac, default_nic_recv, register_nic_driver}, register_driver}, interrupts::{CpuRegisters, register_interrupt_handler}}, debugln, utils::{ptr::CPtr, io::{io_out_u8, io_in_u8, io_out_u32, io_out_u16, io_in_u32, io_in_u16}}};

pub extern "C" fn rtl8139_found(header: PciDeviceHeader, bus: u16, device: u16, function: u16) {
	debugln!("{:?} {}:{}:{}", header, bus, device, function);

	let instance = unsafe {
		let idx = RTL8139_INSTANCES.len();
		RTL8139_INSTANCES.insert(idx, Rtl8139Driver::new(header, bus, device, function));
		RTL8139_INSTANCES.get_mut(idx).unwrap()
	};

	unsafe {
		register_driver(instance as *mut Rtl8139Driver as *mut Driver);
	}
}

extern "C" fn rtl8139_is_device_present(_driver: *mut Driver) -> bool {
	true
}

lazy_static! {
	static ref RTL8139_NAME: CString = CString::new("rtl8139").unwrap();
}

static mut RTL8139_INSTANCES: Vec<Rtl8139Driver> = Vec::new();

extern "C" fn rtl8139_get_device_name(_driver: *mut Driver) -> *const c_char {
	RTL8139_NAME.as_ptr()	
}

fn rtl8139_get_mac(driver: &Rtl8139Driver) -> Mac {
	let mac_part1 = io_in_u32(driver.io_base + 0x00);
	let mac_part2 = io_in_u16(driver.io_base + 0x04);

	Mac {
		mac_p: [
			(mac_part1 >> 0) as u8,
			(mac_part1 >> 8) as u8,
			(mac_part1 >> 16) as u8,
			(mac_part1 >> 24) as u8,
			(mac_part2 >> 0) as u8,
			(mac_part2 >> 8) as u8
		]
	}
}

static RX_BUF_SIZE: u32 = 8192;

fn rtl8139_recieve(driver: &mut Rtl8139Driver) {
	let t = ((driver.rx_buffer.as_ptr() as u32) + driver.current_packet_ptr) as *mut u16;

	let len = unsafe {
		*(((t as u32) + 2) as *mut u16)
	};
	debugln!("rtl8139: received packet of length {}", len);

	let t = ((t as u32) + 4) as *mut u8;

	(driver.driver.recv)(driver as *mut Rtl8139Driver as *mut NicDriver, t, len as u32);

	driver.current_packet_ptr = (driver.current_packet_ptr + (len as u32) + 4 + 3) & (!3);

	if driver.current_packet_ptr > RX_BUF_SIZE {
		driver.current_packet_ptr -= RX_BUF_SIZE;
	}

	io_out_u16(driver.io_base + 0x38, (driver.current_packet_ptr - 0x10) as u16);
}

extern "C" fn rtl8139_interrupt(registers: *mut CpuRegisters, data: CPtr) -> *mut CpuRegisters {
	let driver = unsafe {
		&mut *(data.as_ptr() as *mut Rtl8139Driver)
	};

	let status = io_in_u16(driver.io_base + 0x3e);
	io_out_u16(driver.io_base + 0x3e, 0x5);

	debugln!("rtl8139: interrupt status {}", status);

	if (status & (1 << 2)) != 0 {
		debugln!("rtl8139: Packet sent");
	}

	if (status & (1 << 0)) != 0 {
		debugln!("rtl8139: Received packet");
		rtl8139_recieve(driver);
    }

	registers
}

extern "C" fn rtl8139_init(driver: *mut Driver) {
	let driver = unsafe {
		&mut *(driver as *mut Rtl8139Driver)
	};

	driver.bar_type = (driver.header.bar0 & 1) as u8;
	driver.io_base = (driver.header.bar0 & (!3)) as u16;
	driver.mem_base = (driver.header.bar0 & (!15)) as u32;

	debugln!("rtl8139: bar_type: {}, io_base: {:x}, mem_base: {:x}", driver.bar_type, driver.io_base, driver.mem_base);

	unsafe {
		become_bus_master(driver.bus, driver.device, driver.function);
	}

	io_out_u8(driver.io_base + 0x52, 0);

	io_out_u8(driver.io_base + 0x37, 0x10);
	while (io_in_u8(driver.io_base + 0x37) & 0x10) != 0 {}

	io_out_u32(driver.io_base + 0x30, driver.rx_buffer.as_ptr() as u32);

	// Sets the TOK and ROK bits high
	io_out_u16(driver.io_base + 0x3C, 0x0005);

	// (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	io_out_u32(driver.io_base + 0x44, 0xf | (1 << 7));

	// Sets the RE and TE bits high
	io_out_u8(driver.io_base + 0x37, 0x0C);

	driver.driver.mac = rtl8139_get_mac(driver);
	debugln!("rtl8139: mac: {}", driver.driver.mac);

	unsafe {
		register_interrupt_handler(driver.header.interrupt_line + 0x20, rtl8139_interrupt, CPtr(driver as *mut Rtl8139Driver as *mut c_void));
		register_nic_driver(driver as *mut Rtl8139Driver as *mut NicDriver);
		// load_network_stack(driver as *mut Rtl8139Driver as *mut NicDriver);
	}

	// let mut data: [u8; 8] = [0,1,2,3,4,5,6,7];
	// (driver.driver.send)(driver as *mut Rtl8139Driver as *mut NicDriver, data.as_mut_ptr(), 8);
}

static TSAD_ARRAY: [u16; 4] = [ 0x20, 0x24, 0x28, 0x2C ];
static TSD_ARRAY: [u16; 4] = [ 0x10, 0x14, 0x18, 0x1C ];


extern "C" fn rtl8139_send(driver: *mut NicDriver, data: *mut u8, len: u32) {
	let driver = unsafe {
		&mut *(driver as *mut Rtl8139Driver)
	};
	
	io_out_u32(driver.io_base + TSAD_ARRAY.get(driver.tx_cur as usize).unwrap(), data as u32);
	io_out_u32(driver.io_base + TSD_ARRAY.get(driver.tx_cur as usize).unwrap(), len);

	driver.tx_cur += 1;

	if driver.tx_cur > 3 {
		driver.tx_cur = 0;
	}
}

pub struct Rtl8139Driver {
	pub driver: NicDriver,
	pub header: PciDeviceHeader,
	pub bus: u16,
	pub device: u16,
	pub function: u16,
	pub bar_type: u8,
	pub io_base: u16,
	pub mem_base: u32,
	pub rx_buffer: [u8; 155648],
	pub tx_cur: u32,
	pub current_packet_ptr: u32
}

impl Rtl8139Driver {
	pub fn new(header: PciDeviceHeader, bus: u16, device: u16, function: u16) -> Self {
		Self {
			driver: NicDriver {
				driver: Driver {
					is_device_present: rtl8139_is_device_present,
					get_device_name: rtl8139_get_device_name,
					init: rtl8139_init,
					driver_specific_data: CPtr (0x0 as *mut c_void)
				},
				send: rtl8139_send,
				recv: default_nic_recv,
				ip: Ip {
					ip: 0
				},
				mac: Mac {
					mac: 0
				}
			},
			header,
			bus,
			device,
			function,
			bar_type: 0,
			io_base: 0,
			mem_base: 0,
			rx_buffer: [0; 155648],
			tx_cur: 0,
			current_packet_ptr: 0
		}
	}
}
