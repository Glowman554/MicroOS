all:
	mkdir -p bin
	mkdir -p lib
	make -C libc library
	make -C terminal prog

clean:
	make -C libc clean
	make -C terminal clean