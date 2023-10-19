ENTRY(_start)

SECTIONS
{
	. = 0x200000;
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

    paging_start = .;
    .paging ALIGN(4096) (NOLOAD) : {
        *(.paging)
    }
    paging_end = .;
}