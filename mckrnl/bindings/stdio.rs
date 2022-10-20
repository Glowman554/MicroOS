use core::{fmt, ffi::c_char};
use cstr_core::CString;
use lazy_static::lazy_static;
use spin::Mutex;


extern "C" {
	pub fn printf(s: *const c_char) -> i32;
	pub fn abortf(s: *const c_char) -> i32;
	pub fn _debugf(s: *const c_char) -> i32;
}

pub enum WriterType {
	DISPLAY, DEBUG
}

pub struct Writer {
	writer_type: WriterType
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
		match self.writer_type {
			WriterType::DISPLAY => {
				unsafe {
					printf(CString::new(s).unwrap().as_ptr());
				}
			}
			WriterType::DEBUG => {
				unsafe {
					_debugf(CString::new(s).unwrap().as_ptr());
				}
			}
		}
        Ok(())
    }
}
impl Writer {
	pub fn new(writer_type: WriterType) -> Self {
		Self {
			writer_type
		}
	}
}

lazy_static! {
    pub static ref DISPLAY_WRITER: Mutex<Writer> = Mutex::new(Writer::new(WriterType::DISPLAY));
    pub static ref DEBUG_WRITER: Mutex<Writer> = Mutex::new(Writer::new(WriterType::DEBUG));
}

pub fn _print(args: fmt::Arguments, writer: &Mutex<Writer>) {
    use core::fmt::Write;
    writer.lock().write_fmt(args).unwrap();
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::bindings::stdio::_print(format_args!($($arg)*), &$crate::bindings::stdio::DISPLAY_WRITER));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}}\n", format_args!($($arg)*)));
}

#[macro_export]
macro_rules! debug {
    ($($arg:tt)*) => ($crate::bindings::stdio::_print(format_args!($($arg)*), &$crate::bindings::stdio::DEBUG_WRITER));
}

#[macro_export]
macro_rules! debugln {
    () => ($crate::debug!("\n"));
    ($($arg:tt)*) => ($crate::debug!("[{}:{}] {}\n", file!(), line!(), format_args!($($arg)*)));
}