use core::{mem::size_of, ops::{Index, IndexMut}, ffi::c_void};

#[repr(C)]
pub struct CPtr(pub *mut c_void);
impl CPtr {
	pub fn as_ptr(&self) -> *mut c_void {
		self.0
	}
}
unsafe impl Send for CPtr {}
unsafe impl Sync for CPtr {}

#[repr(C)]
pub struct CPtrArray<T>(pub *mut T);
impl<T> Index<u32> for CPtrArray<T> {
    type Output = T;

    fn index(&self, i: u32) -> &Self::Output {
        unsafe {
            &*(((self.0 as u32) + (size_of::<T>() as u32 * i)) as *mut T)
        }
    }
}
impl<T> IndexMut<u32> for CPtrArray<T> {
    fn index_mut(&mut self, i: u32) -> &mut Self::Output {
        unsafe {
            &mut *(((self.0 as u32) + (size_of::<T>() as u32 * i)) as *mut T)
        }
    }
}
unsafe impl<T> Send for CPtrArray<T> {}
unsafe impl<T> Sync for CPtrArray<T> {}
