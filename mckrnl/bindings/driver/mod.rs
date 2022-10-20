use core::ffi::{c_char};

pub mod clock_driver;

extern {
	pub fn register_driver(driver: *mut Driver);
}

type DriverIsDevicePresend = extern fn(driver: *mut Driver) -> bool;
type DriverGetDeviceName = extern fn(driver: *mut Driver) -> *const c_char;
type DriverInit = extern fn(driver: *mut Driver);

#[repr(C)]
pub struct Driver {
	pub is_device_present: DriverIsDevicePresend,
	pub get_device_name: DriverGetDeviceName,
	pub init: DriverInit,
	pub driver_specific_data: u32
}