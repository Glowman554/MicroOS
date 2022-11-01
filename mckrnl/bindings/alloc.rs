use core::{ffi::c_void, alloc::{GlobalAlloc, Layout}};

// void* vmm_alloc(uint32_t num_pages)
// void vmm_free(void* ptr, uint32_t num_pages)

extern {
	fn vmm_alloc(num_pages: u32) -> *mut c_void;
	fn vmm_free(ptr: *mut c_void, num_pages: u32);
}

/// The global allocator type.
#[derive(Default)]
pub struct Allocator;

unsafe impl GlobalAlloc for Allocator {
	 unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
		vmm_alloc((layout.size() as u32) / 4096 + 1) as *mut u8
	 }
	 unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
		vmm_free(ptr as *mut c_void, (layout.size() as u32) / 4096 + 1);
	 }
}

/// If there is an out of memory error, just panic.
#[alloc_error_handler]
fn allocator_error(_layout: Layout) -> ! {
	panic!("out of memory");
}

/// The static global allocator.
#[global_allocator]
static GLOBAL_ALLOCATOR: Allocator = Allocator;