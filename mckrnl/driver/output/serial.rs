use core::ffi::{c_char, c_int};
use cstr_core::CString;
use lazy_static::lazy_static;
use crate::{bindings::driver::{Driver, char_output_driver::{debugf_driver, CharOutputDriver}}, utils::io::{io_out_u8, io_in_u8}};


extern "C" fn serial_is_device_present(_driver: *mut Driver) -> bool {
	true
}

lazy_static! {
	static ref SERIAL_NAME: CString = CString::new("serial").unwrap();
}

extern "C" fn serial_get_device_name(_driver: *mut Driver) -> *const c_char {
	SERIAL_NAME.as_ptr()	
}

static PORT: u16 = 0x3f8;

extern {
	fn text_console_clrscr();
}

extern "C" fn serial_init(driver: *mut Driver) {
	io_out_u8(PORT + 1, 0x00);    // Disable all interrupts
	io_out_u8(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	io_out_u8(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	io_out_u8(PORT + 1, 0x00);    //                  (hi byte)
	io_out_u8(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	io_out_u8(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	io_out_u8(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	unsafe {
		debugf_driver = driver as *mut CharOutputDriver;
		text_console_clrscr();
	}
}

extern "C" fn is_transmit_empty() -> bool {
	(io_in_u8(PORT + 5) & 0x20) != 0
}

extern "C" fn serial_putc(_driver: *mut CharOutputDriver, c: c_char) {
	while is_transmit_empty() == false {}
	io_out_u8(PORT, c as u8);
}

extern "C" fn serial_vmode(_driver: *mut CharOutputDriver) -> c_int {
	panic!("not implemented");
}

extern "C" fn serial_vpoke(_driver: *mut CharOutputDriver, _offset: u32, _value: u8) {
	panic!("not implemented");
}

pub static SERIAL_DRIVER: CharOutputDriver = CharOutputDriver {
	driver: Driver {
		is_device_present: serial_is_device_present,
		get_device_name: serial_get_device_name,
		init: serial_init,
		driver_specific_data: 0
	},
	putc: serial_putc,
	vmode: serial_vmode,
	vpoke: serial_vpoke
};