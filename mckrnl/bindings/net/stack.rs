use crate::bindings::driver::nic_driver::NicDriver;

extern {
	pub fn load_network_stack(driver: *mut NicDriver);
}