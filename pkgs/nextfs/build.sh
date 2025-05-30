if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/Glowman554/nextfs-loader src --recursive
fi


if [ -d "installer" ]; then
	rm -rvf installer
fi

make -C src/loader
make -C src/kernel
make -f $1 -C tool PROGRAM=nextfs.elf AUTHOR=glowman554 LIBS_ZIP=$2 extract_libs prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.mex ../pkgs/nextfs_install.mex