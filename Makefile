all:
	make -C krnl
	make -C user
	make -C user install

iso: all
	cp krnl/krnl.elf cdrom/.
	cp LICENSE cdrom/.
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o cdrom.iso cdrom/

run: iso
	qemu-system-i386 -cdrom cdrom.iso -boot d -serial stdio --no-reboot --no-shutdown


run_dbg: iso
	qemu-system-i386 -cdrom cdrom.iso -boot d -serial stdio --no-reboot --no-shutdown -s -S

debug:
	gdb -ex "target remote localhost:1234" -ex "b init" -ex "continue" krnl/krnl.elf

clean: iso
	make -C krnl clean
	make -C user clean
