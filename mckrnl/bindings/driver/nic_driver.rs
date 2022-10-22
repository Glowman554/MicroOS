use core::fmt::{Formatter, Display, Result};
use crate::debugln;

use super::Driver;

#[repr(C)]
pub union Ip {
	pub ip_p: [u8; 4],
	pub ip: u32
}
impl Display for Ip {
	fn fmt(&self, f: &mut Formatter) -> Result {
		unsafe {
			write!(f, "{}.{}.{}.{}", self.ip_p[0], self.ip_p[1], self.ip_p[2], self.ip_p[3])
		}
	}
}

#[repr(C)]
pub union Mac {
	pub mac_p: [u8; 6],
	pub mac: u64
}
impl Display for Mac {
	fn fmt(&self, f: &mut Formatter) -> Result {
		unsafe {
			write!(f, "{:2x}:{:2x}:{:2x}:{:2x}:{:2x}:{:2x}", self.mac_p[0], self.mac_p[1], self.mac_p[2], self.mac_p[3], self.mac_p[4], self.mac_p[5])
		}
	}
}

type NicSend = extern fn(driver: *mut NicDriver, data: *mut u8, len: u32);
type NicRecv = extern fn(driver: *mut NicDriver, data: *mut u8, len: u32);

#[repr(C)]
pub struct NicDriver {
	pub driver: Driver,

	pub send: NicSend,
	pub recv: NicRecv, // this is a callback function CALLED BY THE DRIVER!

	pub ip: Ip,
	pub mac: Mac
}

extern {
	pub fn register_nic_driver(driver: *mut NicDriver);
}

pub extern "C" fn default_nic_recv(_driver: *mut NicDriver, _data: *mut u8, len: u32) {
	debugln!("recieved {} bytes!", len);
}