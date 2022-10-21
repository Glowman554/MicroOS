use crate::utils::ptr::CPtr;

#[repr(C)]
pub struct CpuRegisters {
	pub eax: u32,
	pub ebx: u32,
	pub ecx: u32,
	pub edx: u32,
	pub esi: u32,
	pub edi: u32,
	pub ebp: u32,

	pub intr: u32,
	pub error: u32,

	pub eip: u32,
	pub cs: u32,
	pub eflags: u32,
	pub esp: u32,
	pub ss: u32
}

type InterruptHandler = extern fn(registers: *mut CpuRegisters, data: CPtr) -> *mut CpuRegisters;

extern {
	pub fn register_interrupt_handler(intr: u8, handler: InterruptHandler, data: CPtr);
}