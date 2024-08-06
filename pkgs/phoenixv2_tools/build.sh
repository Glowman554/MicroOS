if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/GLowman554/phoenix-v2 src
    (
        cd src
        git am < ../0001-fix-disasm.patch
        git am < ../0002-fix-visual-emulator.patch
    )
fi


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $1 -C src/tools/assemblerv2 PROGRAM=phoenixv2-as.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/disassembler PROGRAM=phoenixv2-disasm.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/emulator PROGRAM=phoenixv2-emu.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog
make -f $1 -C src/tools/visual_eumlator PROGRAM=phoenixv2-emu-visual.elf LIBS_ZIP=$2 USER_CFLAGS=-I../common extract_libs prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.elf ../pkgs/phoenixv2_tools_install.elf