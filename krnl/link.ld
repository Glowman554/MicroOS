ENTRY(_start)

SECTIONS
{
	. = 0x100000;
	kernel_start = .;
	
	.text : {
		*(multiboot)
		*(.text)
	}
	.data ALIGN(4096) : {
		start_ctors = .;
		KEEP(*( .init_array ));
		KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
		end_ctors = .;
		*(.data)
	}
	.rodata ALIGN(4096) : {
		*(.rodata)
	}
	.bss ALIGN(4096) : {
		*(.bss)
	}
	. = ALIGN(4096);
	kernel_end = .;
}