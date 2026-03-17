mkdir -p installer
python3 $INSTALLER_PY pkg.json installer/install.c
make -f $PROGRAM_MK -C installer PROGRAM=install.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP EXTRA_OBJS=./res/libs/libtinf.o extract_libs prog