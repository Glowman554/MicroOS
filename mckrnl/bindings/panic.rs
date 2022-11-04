use core::panic::PanicInfo;
extern crate alloc;
use alloc::format;
use cstr_core::CString;
use crate::bindings::stdio::abortf;


#[panic_handler]
fn panic(info: &PanicInfo<'_>) -> ! {
	unsafe {
		abortf(CString::new(format!("Rust panic: {}", info)).unwrap().as_ptr());
	}
	loop {}
}