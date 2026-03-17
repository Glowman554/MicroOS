if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $PROGRAM_MK -C make PROGRAM=make.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP USER_CFLAGS=-I../include extract_libs compile_flags.txt prog
make -f $PROGRAM_MK -C extract PROGRAM=extract.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP USER_CFLAGS=-I../include extract_libs compile_flags.txt prog

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/saf_tools_install.mex