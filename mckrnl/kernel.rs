#![no_std]
#![feature(alloc_error_handler)]

pub mod bindings;

#[no_mangle]
extern "C" fn rust_main() {
	debugln!("Hello rusty world!\n");
}