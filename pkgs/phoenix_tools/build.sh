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


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $1 -C src/tools/assemblerv2 PROGRAM=phoenix-as.elf AUTHOR=glowman554 LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/disassembler PROGRAM=phoenix-disasm.elf AUTHOR=glowman554 LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/emulator PROGRAM=phoenix-emu.elf AUTHOR=glowman554 LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.mex ../pkgs/phoenix_tools_install.mex