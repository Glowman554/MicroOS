mkdir -p installer
python3 $3 pkg.json installer/install.c
make -f $1 -C installer PROGRAM=install.elf AUTHOR=glowman554 LIBS_ZIP=$2 EXTRA_OBJS=./res/libs/libtinf.o extract_libs prog