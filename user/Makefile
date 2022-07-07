all:
	mkdir -p bin
	mkdir -p lib
	make -C libc library
	make -C terminal prog
	make -C test prog

clean:
	make -C libc clean
	make -C terminal clean
	make -C test clean