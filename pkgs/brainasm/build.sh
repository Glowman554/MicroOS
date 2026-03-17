if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/Glowman554/BrainASM src
fi


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $PROGRAM_MK -C src/src PROGRAM=brainasm.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP extract_libs
make -f $PROGRAM_MK -C src/src PROGRAM=brainasm.elf AUTHOR=glowman554 LIBS_ZIP=$LIBS_ZIP prog -j $(nproc)

bash ../run_installer.sh
cp -v installer/install.mex ../pkgs/brainasm_install.mex