include config.mk

all: res
	make -C mckrnl
	make -C user
ifeq ($(GUI),1)
	make -C gui
endif

NETDEV = e1000

QEMU_FLAGS = -m 2G -cdrom cdrom.iso -boot d -hda res/foxos.img
QEMU_FLAGS += -netdev user,id=u1 -device $(NETDEV),netdev=u1 -object filter-dump,id=f1,netdev=u1,file=dump.pcap
# QEMU_FLAGS += -soundhw pcspk
ifeq ($(AUDIO),1)
QEMU_FLAGS += -audio driver=pipewire,model=ac97
endif
QEMU_FLAGS += -smp 4

ifdef REMOTE
	QEMU_FLAGS += -serial tcp:127.0.0.1:1234,server
else
	QEMU_FLAGS += -serial stdio
endif

ifdef AHCI
	QEMU_FLAGS += -machine q35
endif

initrd.saf:
	mkdir -p ./res/initrd/bin
	cp -r ./user/bin/*.mex ./res/initrd/bin/ -v
ifeq ($(GUI),1)
	cp -r ./gui/bin/*.mex ./res/initrd/bin/ -v
	mkdir -p ./res/initrd/opt/gui/bin
	cp -r ./gui/bin/*.flbb ./res/initrd/opt/gui/bin/ -v
endif
	cp -r ./initrd/* ./res/initrd/ -v
	cp LICENSE ./res/initrd/LICENSE -v
	mkdir -p ./res/initrd/docs
	cp *.md ./res/initrd/docs/. -v
	mkdir -p ./res/initrd/EFI/BOOT
	cp mckrnl/mckrnl.* ./res/initrd/EFI/BOOT/. -v
	./res/saf/saf-make ./res/initrd ./res/initrd.saf

iso: all initrd.saf
	cp mckrnl/mckrnl.elf cdrom/.
	cp mckrnl/mckrnl.syms cdrom/.
	cp res/initrd.saf cdrom/.
	cp LICENSE cdrom/.
	grub-mkrescue -o cdrom.iso cdrom/

set_kvm:
ifneq ("$(wildcard ./kvm)","")
	@echo "enabling kvm"
	$(eval QEMU_FLAGS += --enable-kvm)
endif

run: iso set_kvm
	qemu-system-i386 $(QEMU_FLAGS) -s

test: iso
	make -C test

res:
	mkdir res
	git clone https://github.com/chocabloc/saf.git --depth=1 ./res/saf
	make -C res/saf

	wget https://github.com/TheUltimateFoxOS/FoxOS/releases/download/latest/foxos.img -O res/foxos.img

format_disk:
	dd if=/dev/zero of=res/foxos.img bs=512 count=93750 status=progress
	mkfs.vfat -F 32 res/foxos.img

format_disk_gpt:
	dd if=/dev/zero of=res/foxos.img bs=512 count=93750 status=progress
	echo 'echo "o\ny\nn\n1\n\n\n0700\nw\ny\n" | gdisk res/foxos.img' | sh
	sudo losetup /dev/loop100 res/foxos.img -P
	sudo mkfs.vfat -F 32 /dev/loop100p1
	sudo losetup -d /dev/loop100

run_dbg: iso
	qemu-system-i386 $(QEMU_FLAGS) --no-reboot --no-shutdown -s -S

run_vnc: iso set_kvm
	qemu-system-i386 $(QEMU_FLAGS) -s -vnc :1

EXECUTABLE = mckrnl/mckrnl.elf

debug:
	gdb -ex "symbol-file $(EXECUTABLE)" -ex "target remote localhost:1234" -ex "b _main"

clean: iso
	make -C mckrnl clean
	make -C user clean
ifeq ($(GUI),1)
	make -C gui clean
endif

deepclean:
	rm -rfv res
	git clean -dxf

libs.zip: all
	mkdir -p res/libs/include
	cp user/lib/* res/libs/. -rf
	cp user/libc/include/* res/libs/include/. -rf
	cp user/libtinf/include/* res/libs/include/. -rf
	cp user/libflvm/include/* res/libs/include/. -rf
	cp user/libjson/include/* res/libs/include/. -rf

ifeq ($(GUI),1)
	cp gui/lib/* res/libs/. -rf
	cp gui/libwindow/include/* res/libs/include/. -rf
endif

	zip -r libs.zip res/libs/

compile_flags.txt:
	make -C mckrnl compile_flags.txt 
	make -C user compile_flags.txt
ifeq ($(GUI),1)
	make -C gui compile_flags.txt
endif

pre_commit:
	deno run -A config/write_syscalls_md.ts
	deno run -A config/config.ts --clean --auto config/libc.json
	deno run -A config/config.ts --clean --auto config/kernel.json
	deno run -A config/config.ts --clean --auto --mode makefile config/build.json
	cd pkgs; bash clean.sh


config_libc:
	deno run -A config/config.ts config/libc.json

config_kernel:
	deno run -A config/config.ts config/kernel.json

config_build:
	deno run -A config/config.ts --mode makefile config/build.json