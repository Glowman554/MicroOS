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