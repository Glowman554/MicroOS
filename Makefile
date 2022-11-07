all:
	make -C mckrnl
	make -C user

KEYMAP = de
NETDEV = rtl8139

QEMU_FLAGS = -m 1G -cdrom cdrom.iso -boot d -serial stdio -hda res/foxos.img
QEMU_FLAGS += -netdev user,id=u1 -device $(NETDEV),netdev=u1 -object filter-dump,id=f1,netdev=u1,file=dump.dat

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
	qemu-system-i386 $(QEMU_FLAGS) -s

test: iso
	make -C test

res:
	mkdir res
	git clone https://github.com/chocabloc/saf.git --depth=1 ./res/saf
	make -C res/saf

	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/de.fmp -O res/de.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/us.fmp -O res/us.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/raw/main/disk_resources/resources/fr.fmp -O res/fr.fmp
	wget https://github.com/TheUltimateFoxOS/FoxOS/releases/download/latest/foxos.img -O res/foxos.img


run_dbg: iso
	qemu-system-i386 $(QEMU_FLAGS) --no-reboot --no-shutdown -s -S

EXECUTABLE = mckrnl/mckrnl.elf

debug:
	gdb -ex "target remote localhost:1234" -ex "b _main" -ex "continue" $(EXECUTABLE)

clean: iso
	make -C mckrnl clean
	make -C user clean
