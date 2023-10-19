mkdir -p foxtail
python3 $3 pkg.json foxtail/install.c
make -f $1 -C foxtail PROGRAM=install.elf LIBS_ZIP=$2 EXTRA_OBJS=./res/libs/libtinf.o extract_libs prog