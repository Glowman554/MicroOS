if [ -d "src" ]; then
	echo "no need to clone again"
else
	echo "cloning..."
	git clone https://github.com/GLowman554/FireStorm src
	(
		cd src
		deno run -A src/index.ts -t bytecode -o hello.flbb tests/hello.fl
		deno run -A src/index.ts -t bytecode -o powers_of_two.flbb tests/powers_of_two.fl
		deno run -A src/index.ts -t bytecode -o fib_itterative.flbb tests/fib_itterative.fl
	)
fi


if [ -d "foxtail" ]; then
	rm -rvf foxtail
fi

make -f $1 -C src/src PROGRAM=flvm.elf LIBS_ZIP=$2 extract_libs prog

bash ../run_foxtail.sh $1 $2 $3
cp -v foxtail/install.elf ../pkgs/flvm_install.elf