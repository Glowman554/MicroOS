
LIBS = libc libtinf libflvm libjson
PROGRAMS = init terminal ls cat test echo calc pwr edit clear \
           ping nslookup play wc cp env file mkdir rm rmdir date background \
		   fasm figl du linux tasks stream free install lminst touch dhcp ip \
		   pciinfo flvm scheduler if httpget jsonprint createmex lolcat load

all:
	make build_libraries -j $(nproc)
	make build_programs -j $(nproc)

build_libraries: $(addsuffix _library,$(LIBS))
build_programs: $(addsuffix _prog,$(PROGRAMS))

%_library:
	mkdir -p lib
	make -C $* library -j $(nproc)

%_prog:
	mkdir -p bin
	make -C $* prog -j $(nproc)


clean:
	$(foreach lib,$(LIBS),make -C $(lib) clean;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) clean;)

compile_flags.txt:
	$(foreach lib,$(LIBS),make -C $(lib) compile_flags.txt;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) compile_flags.txt;)