all:
	make -C core 

	mkdir -p modules
	make -C filesystem/fat32
	make -C filesystem/nextfs
	make -C filesystem/ramfs
	make -C driver/ac97
	make -C driver/ata
	make -C driver/ps2

	make -C network/am79C973
	make -C network/e1000
	make -C network/ne2k
	make -C network/rtl8139

	make -C feature/acpi_dump


clean:
	make -C core clean

	make -C filesystem/fat32 clean
	make -C filesystem/nextfs clean
	make -C filesystem/ramfs clean
	make -C driver/ac97 clean
	make -C driver/ata clean
	make -C driver/ps2 clean

	make -C network/am79C973 clean
	make -C network/e1000 clean
	make -C network/ne2k clean
	make -C network/rtl8139 clean

	make -C feature/acpi_dump clean

compile_flags.txt:
	make -C core compile_flags.txt 
	
	make -C filesystem/fat32 compile_flags.txt
	make -C filesystem/nextfs compile_flags.txt
	make -C filesystem/ramfs compile_flags.txt
	make -C driver/ac97 compile_flags.txt
	make -C driver/ata compile_flags.txt
	make -C driver/ps2 compile_flags.txt

	make -C network/am79C973 compile_flags.txt
	make -C network/e1000 compile_flags.txt
	make -C network/ne2k compile_flags.txt
	make -C network/rtl8139 compile_flags.txt

	make -C feature/acpi_dump compile_flags.txt