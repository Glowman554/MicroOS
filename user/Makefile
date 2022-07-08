all:
	mkdir -p bin
	mkdir -p lib
	make -C libc library
	make -C init prog
	make -C terminal prog
	make -C test prog

clean:
	make -C libc clean
	make -C init clean
	make -C terminal clean
	make -C test clean