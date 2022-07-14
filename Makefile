all:
	make -C mckrnl
	make -C user

KEYMAP = de

initrd.saf:
	mkdir -p ./res/initrd/bin
	cp -r ./user/bin/*.elf ./res/initrd/bin/ -v
	cp ./res/$(KEYMAP).fmp ./res/initrd/keymap.fmp -v
	cp -r ./initrd/* ./res/initrd/ -v
	cp LICENSE ./res/initrd/LICENSE -v
	cp *.md ./res/initrd/. -v
	./res/saf/saf-make ./res/initrd ./res/initrd.saf

iso: all initrd.saf
	cp mckrnl/mckrnl.elf cdrom/.
	cp res/initrd.saf cdrom/.
	cp LICENSE cdrom/.
	grub-mkrescue -o cdrom.iso cdrom/

run: iso
	qemu-system-i386 -m 1G -cdrom cdrom.iso -boot d -serial stdio -hda res/foxos.img

res:
	mkdir res
	git clone https://github.com/chocabloc/saf.git --depth=1 ./res/saf
	make -C res/saf

	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/de.fmp -O res/de.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/us.fmp -O res/us.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/fr.fmp -O res/fr.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/releases/download/latest/foxos.img -O res/foxos.img


run_dbg: iso
	qemu-system-i386 -m 1G -cdrom cdrom.iso -boot d -serial stdio --no-reboot --no-shutdown -hda res/foxos.img -s -S

EXECUTABLE = mckrnl/mckrnl.elf

debug:
	gdb -ex "target remote localhost:1234" -ex "b main" -ex "continue" $(EXECUTABLE)

clean: iso
	make -C mckrnl clean
	make -C user clean
