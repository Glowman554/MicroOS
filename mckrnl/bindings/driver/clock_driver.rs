use crate::bindings::driver::Driver;
use core::ffi::c_int;

#[repr(C)]
#[derive(Debug)]
pub struct ClockResult {
	pub seconds: c_int,
	pub minutes: c_int,
	pub hours: c_int,
	pub day: c_int,
	pub month: c_int,
	pub year: c_int
}

type ClockGetTime = extern fn(driver: *mut ClockDriver) -> ClockResult;

#[repr(C)]
pub struct ClockDriver {
	pub driver: Driver,
	pub get_time: ClockGetTime
}

extern "C" {
    pub static mut global_clock_driver: *mut ClockDriver;
}