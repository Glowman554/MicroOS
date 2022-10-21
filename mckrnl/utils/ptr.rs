use core::ffi::c_void;

#[repr(C)]
pub struct CPtr(pub *mut c_void);
impl CPtr {
	pub fn as_ptr(&self) -> *mut c_void {
		self.0
	}
}
unsafe impl Send for CPtr {}
unsafe impl Sync for CPtr {}
