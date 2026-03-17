if [ -f "nyan.bin" ]; then
	echo "no need to download again"
else
	echo "downloading..."
	wget https://github.com/TheUltimateFoxOS/foxtail/raw/main/fun/progs/nyanmbr/nyan.bin
fi


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $PROGRAM_MK -C src PROGRAM=nyanmbr.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP USER_CFLAGS=-I../common extract_libs prog

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/nyanmbr_install.mex