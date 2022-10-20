#![no_std]
#![feature(alloc_error_handler)]

pub mod bindings;

#[no_mangle]
extern "C" fn rust_register_pci_drivers() {
	debugln!("in rust_register_pci_drivers");
}

#[no_mangle]
extern "C" fn rust_register_drivers() {
	debugln!("in rust_register_drivers");
}

#[no_mangle]
extern "C" fn rust_register_fs_scanners() {
	debugln!("in rust_register_fs_scanners");
}

#[no_mangle]
extern "C" fn rust_main() {
	debugln!("Hello rusty world!");
}
