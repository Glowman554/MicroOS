#![no_std]
#![feature(alloc_error_handler)]

pub mod bindings;
pub mod driver;
pub mod utils;

#[no_mangle]
extern "C" fn rust_register_pci_drivers() {
	debugln!("in rust_register_pci_drivers");
}

use crate::{driver::{clock::cmos::CMOS_DRIVER, output::serial::SERIAL_DRIVER}, bindings::driver::{register_driver, Driver, clock_driver::ClockDriver, char_output_driver::CharOutputDriver}};

#[no_mangle]
extern "C" fn rust_register_drivers() {
	debugln!("in rust_register_drivers");

	unsafe {
		register_driver(&SERIAL_DRIVER as *const CharOutputDriver as *mut Driver);
		register_driver(&CMOS_DRIVER as *const ClockDriver as *mut Driver);
	}
}

#[no_mangle]
extern "C" fn rust_register_fs_scanners() {
	debugln!("in rust_register_fs_scanners");
}

#[no_mangle]
extern "C" fn rust_main() {
	debugln!("Hello rusty world!");
}
