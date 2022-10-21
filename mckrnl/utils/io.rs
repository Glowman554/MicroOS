use core::arch::asm;

pub fn io_out_u8(addr: u16, val: u8) {
	unsafe {
		asm!(
			"out dx, al",
			in("dx") addr,
			in("al") val,
			options(preserves_flags, nomem, nostack)
		);
	}
}

pub fn io_out_u16(addr: u16, val: u16) {
	unsafe {
		asm!(
			"out dx, ax",
			in("dx") addr,
			in("ax") val,
			options(preserves_flags, nomem, nostack)
		);
	}
}

pub fn io_out_u32(addr: u16, val: u32) {
	unsafe {
		asm!(
			"out dx, eax",
			in("dx") addr,
			in("eax") val,
			options(preserves_flags, nomem, nostack)
		);
	}
}

pub fn io_in_u8(addr: u16) -> u8 {
	let ret: u8;
	unsafe {
		asm!(
			"in al, dx",
			in("dx") addr,
			out("al") ret,
			options(preserves_flags, nomem, nostack)
		);
	}

	ret
}

pub fn io_in_u16(addr: u16) -> u16 {
	let ret: u16;
	unsafe {
		asm!(
			"in ax, dx",
			in("dx") addr,
			out("ax") ret,
			options(preserves_flags, nomem, nostack)
		);
	}

	ret
}

pub fn io_in_u32(addr: u16) -> u32 {
	let ret: u32;
	unsafe {
		asm!(
			"in eax, dx",
			in("dx") addr,
			out("eax") ret,
			options(preserves_flags, nomem, nostack)
		);
	}

	ret
}