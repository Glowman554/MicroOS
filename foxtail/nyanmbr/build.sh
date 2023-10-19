if [ -f "nyan.bin" ]; then
	echo "no need to download again"
else
	echo "downloading..."
	wget https://github.com/TheUltimateFoxOS/foxtail/raw/main/fun/progs/nyanmbr/nyan.bin
fi


if [ -d "foxtail" ]; then
	rm -rvf foxtail
fi

make -f $1 -C src PROGRAM=nyanmbr.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog

bash ../run_foxtail.sh $1 $2 $3
cp -v foxtail/install.elf ../pkgs/nyanmbr_install.elf