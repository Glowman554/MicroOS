rm -rf res
make run
qemu-system-i386 -hda res/hda.img -serial stdio
