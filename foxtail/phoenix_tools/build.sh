if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/Nudeltruppe/CpuV2 src
    (
        cd src
        git am < ../0001-fix-disasm.patch
    )
fi


if [ -d "foxtail" ]; then
	rm -rvf foxtail
fi

make -f $1 -C src/tools/assemblerv2 PROGRAM=phoenix-as.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/disassembler PROGRAM=phoenix-disasm.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/emulator PROGRAM=phoenix-emu.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog

bash ../run_foxtail.sh $1 $2 $3
cp -v foxtail/install.elf ../pkgs/phoenix_tools_install.elf