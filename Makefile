all:
	make -C krnl
	make -C user
	make -C user install

KEYMAP = de

initrd:
	mkdir -p ./res/initrd/bin
	cp -r ./user/*.elf ./res/initrd/bin/ -v
	cp ./res/$(KEYMAP).fmp ./res/initrd/keymap.fmp -v
	./res/saf/saf-make ./res/initrd ./res/initrd.saf

iso: all initrd
	cp krnl/krnl.elf cdrom/.
	cp res/initrd.saf cdrom/.
	cp LICENSE cdrom/.
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso cdrom/

run: iso
	qemu-system-i386 -cdrom cdrom.iso -boot d -serial stdio --no-reboot --no-shutdown

res:
	mkdir res
	git clone https://github.com/chocabloc/saf.git --depth=1 ./res/saf
	make -C res/saf

	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/de.fmp -O res/de.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/us.fmp -O res/us.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/fr.fmp -O res/fr.fmp


run_dbg: iso
	qemu-system-i386 -cdrom cdrom.iso -boot d -serial stdio --no-reboot --no-shutdown -s -S

debug:
	gdb -ex "target remote localhost:1234" -ex "b init" -ex "continue" krnl/krnl.elf

clean: iso
	make -C krnl clean
	make -C user clean
