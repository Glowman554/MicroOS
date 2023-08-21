if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/Glowman554/BrainASM src
fi


if [ -d "foxtail" ]; then
	rm -rvf foxtail
fi

make -f $1 -C src/src PROGRAM=brainasm.elf LIBS_ZIP=$2 extract_libs prog

bash ../run_foxtail.sh $1 $2 $3
cp -v foxtail/install.elf ../pkgs/brainasm_install.elf