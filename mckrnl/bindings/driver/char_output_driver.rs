use core::ffi::{c_int, c_char};

use crate::bindings::driver::Driver;

pub enum VideoMode {
	Text80X25
}

type CharOutputDriverPutc = extern fn(driver: *mut CharOutputDriver, c: c_char);
type CharOutputDriverVmode = extern fn(driver: *mut CharOutputDriver) -> c_int;
type CharOutputDriverVpoke = extern fn(driver: *mut CharOutputDriver, offset: u32, value: u8);

#[repr(C)]
pub struct CharOutputDriver {
	pub driver: Driver,
	pub putc: CharOutputDriverPutc,
	pub vmode: CharOutputDriverVmode,
	pub vpoke: CharOutputDriverVpoke
}

extern "C" {
    pub static mut global_char_output_driver: *mut CharOutputDriver;
    pub static mut debugf_driver: *mut CharOutputDriver;
    pub static mut printf_driver: *mut CharOutputDriver;
}