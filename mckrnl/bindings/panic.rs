use core::panic::PanicInfo;
use cstr_core::CString;
use crate::{debugln, bindings::stdio::abortf};



#[panic_handler]
fn panic(info: &PanicInfo<'_>) -> ! {
	debugln!("Rust panic: {}", info);
	unsafe {
		abortf(CString::new("Rust panic! More information is in the serial log.").unwrap().as_ptr());
	}
	loop {}
}