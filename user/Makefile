all:
	mkdir -p bin
	mkdir -p lib
	make -C libc library

	make -C init prog
	make -C terminal prog
	make -C ls prog
	make -C cat prog
	make -C test prog
	make -C echo prog
	make -C calc prog
	make -C pwr prog
	make -C edit prog
	make -C clear prog
	make -C ping prog
	make -C nslookup prog
	make -C play prog
	make -C wc prog
	make -C cp prog
	make -C env prog
	make -C file prog

clean:
	make -C libc clean
	
	make -C init clean
	make -C terminal clean
	make -C ls clean
	make -C cat clean
	make -C test clean
	make -C echo clean
	make -C calc clean
	make -C pwr clean
	make -C edit clean
	make -C clear clean
	make -C ping clean
	make -C nslookup clean
	make -C play clean
	make -C wc clean
	make -C cp clean
	make -C env clean
	make -C file clean

compile_flags.txt:
	make -C libc compile_flags.txt

	make -C init compile_flags.txt
	make -C terminal compile_flags.txt
	make -C ls compile_flags.txt
	make -C cat compile_flags.txt
	make -C test compile_flags.txt
	make -C echo compile_flags.txt
	make -C calc compile_flags.txt
	make -C pwr compile_flags.txt
	make -C edit compile_flags.txt
	make -C clear compile_flags.txt
	make -C ping compile_flags.txt
	make -C nslookup compile_flags.txt
	make -C play compile_flags.txt
	make -C wc compile_flags.txt
	make -C cp compile_flags.txt
	make -C env compile_flags.txt
	make -C file compile_flags.txt