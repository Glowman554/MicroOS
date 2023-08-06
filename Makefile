all: res
	make -C mckrnl
	make -C user

NETDEV = rtl8139

QEMU_FLAGS = -m 1G -cdrom cdrom.iso -boot d -serial stdio -hda res/foxos.img
QEMU_FLAGS += -netdev user,id=u1 -device $(NETDEV),netdev=u1 -object filter-dump,id=f1,netdev=u1,file=dump.dat
QEMU_FLAGS += -soundhw pcspk
QEMU_FLAGS += -smp 1

initrd.saf:
	mkdir -p ./res/initrd/bin
	cp -r ./user/bin/*.elf ./res/initrd/bin/ -v
	cp -r ./initrd/* ./res/initrd/ -v
	cp LICENSE ./res/initrd/LICENSE -v
	cp *.md ./res/initrd/. -v
	./res/saf/saf-make ./res/initrd ./res/initrd.saf

iso: all initrd.saf
	cp mckrnl/mckrnl.elf cdrom/.
	cp mckrnl/mckrnl.syms cdrom/.
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

	wget https://github.com/TheUltimateFoxOS/FoxOS/releases/download/latest/foxos.img -O res/foxos.img


run_dbg: iso
	qemu-system-i386 $(QEMU_FLAGS) --no-reboot --no-shutdown -s -S

EXECUTABLE = mckrnl/mckrnl.elf

debug:
	gdb -ex "target remote localhost:1234" -ex "b _main" -ex "continue" $(EXECUTABLE)

clean: iso
	make -C mckrnl clean
	make -C user clean

deepclean:
	rm -rfv res
	git clean -dxf

libs.zip: all
	mkdir -p res/libs/include
	cp user/lib/* res/libs/. -rf
	cp user/libc/include/* res/libs/include/. -rf

	zip -r libs.zip res/libs/

compile_flags.txt:
	make -C mckrnl compile_flags.txt 
	make -C user compile_flags.txt

pre_commit:
	deno run -A config/write_syscalls_md.ts
	deno run -A config/config.ts --clean --auto config/libc.json
	deno run -A config/config.ts --clean --auto config/kernel.json

config_libc:
	deno run -A config/config.ts config/libc.json

config_kernel:
	deno run -A config/config.ts config/kernel.json