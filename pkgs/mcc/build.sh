if [ -d "installer" ]; then
	rm -rvf installer
fi

make -f $1 -C src PROGRAM=mcc.elf AUTHOR=glowman554 LIBS_ZIP=$2 extract_libs prog

bash ../run_installer.sh $1 $2 $3
cp -v installer/install.mex ../pkgs/mcc_install.mex