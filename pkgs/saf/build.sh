if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $1 -C make PROGRAM=make.elf LIBS_ZIP=$2 USER_CFLAGS=-I../include extract_libs compile_flags.txt prog
make -f $1 -C extract PROGRAM=extract.elf LIBS_ZIP=$2 USER_CFLAGS=-I../include extract_libs compile_flags.txt prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.elf ../pkgs/saf_tools_install.elf