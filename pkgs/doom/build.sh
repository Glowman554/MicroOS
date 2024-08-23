

if [ -d "doom_src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/Daivuk/PureDOOM doom_src
	(
		cd doom_src
		git am < ../0001-fix-backspace-and-enter-keys.patch
		git am < ../0002-allow-ROOT_FS-instead-of-HOME-and-DOOMWADDIR.patch
	)
    cp doom_src/src/DOOM src/ -rfv
fi


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -C src LIBS_ZIP=$2 compile_flags.txt extract_libs prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.elf ../pkgs/doom_install.elf