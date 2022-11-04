use core::ffi::{c_char, c_void};

use crate::{bindings::driver::{Driver, clock_driver::{ClockResult, ClockDriver, global_clock_driver}}, utils::{io::{io_out_u8, io_in_u8}, ptr::CPtr}, debugln};
use cstr_core::cstr;

static CMOS_READ_SEC: u8 =		0x00;
static CMOS_READ_MIN: u8 =		0x02;
static CMOS_READ_HOUR: u8 =		0x04;
// static CMOD_READ_WEEKDAY: u8 =	0x06;
static CMOS_READ_DAY: u8 =		0x07;
static CMOS_READ_MONTH: u8 =	0x08;
static CMOS_READ_YEAR: u8 =		0x09;
static CMOS_READ_CENTURY: u8 =	0x32;


extern "C" fn cmos_is_device_present(_driver: *mut Driver) -> bool {
	true
}

extern "C" fn cmos_get_device_name(_driver: *mut Driver) -> *const c_char {
	cstr!("cmos").as_ptr()
}

fn cmos_read(addr: u8) -> u8 {
	if addr < 10 {
		io_out_u8(0x70, 0xa);

		while (io_in_u8(0x71) & (1 << 7)) != 0 {}
	}

	io_out_u8(0x70, addr);
	io_in_u8(0x71)
}

fn cmos_convert(num: u8) -> u8 {
	if (cmos_read(0xb) & (1 << 2)) == 0 {
		(num & 0xf) + ((num >> 4) & 0xf) * 10
	} else {
		num
	}
}

fn cmos_command(fun: u8) -> u8 {
	cmos_convert(cmos_read(fun))
}

extern "C" fn cmos_get_time(_driver: *mut ClockDriver) -> ClockResult {
	let century = cmos_command(CMOS_READ_CENTURY) as i32;

	ClockResult {
		seconds: cmos_command(CMOS_READ_SEC) as i32,
		minutes: cmos_command(CMOS_READ_MIN) as i32,
		hours: cmos_command(CMOS_READ_HOUR) as i32,
		day: cmos_command(CMOS_READ_DAY) as i32,
		month: cmos_command(CMOS_READ_MONTH) as i32,
		year: (cmos_command(CMOS_READ_YEAR) as i32) + (century * 100)
	}
}

extern "C" fn cmos_init(driver: *mut Driver) {
	unsafe {
		global_clock_driver = driver as *mut ClockDriver;
	}

	let driver: &mut ClockDriver = unsafe {
		&mut *(driver as *mut ClockDriver)
	};

	let result = (driver.get_time)(driver as *mut ClockDriver);
	debugln!("{:?}", result);
}

pub static CMOS_DRIVER: ClockDriver = ClockDriver {
	driver: Driver {
		is_device_present: cmos_is_device_present,
		get_device_name: cmos_get_device_name,
		init: cmos_init,
		driver_specific_data: CPtr(0x0 as *mut c_void)
	},
	get_time: cmos_get_time
};