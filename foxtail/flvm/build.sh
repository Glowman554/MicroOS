if [ -d "foxtail" ]; then
	rm -rvf foxtail
fi

make -f $1 -C src PROGRAM=flvm.elf LIBS_ZIP=$2 extract_libs prog

(
	#cd example
	#flp build
)

bash ../run_foxtail.sh $1 $2 $3
cp -v foxtail/install.elf ../pkgs/flvm_install.elf