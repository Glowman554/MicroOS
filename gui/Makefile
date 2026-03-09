LIBS = libwindow libwindow_flvm
PROGRAMS = imgview paint desktop

all:
	mkdir -p lib
	$(foreach lib,$(LIBS),make -C $(lib) library;)
	make build_programs -j $(nproc)

build_programs: $(addsuffix _prog,$(PROGRAMS))

%_prog:
	mkdir -p bin
	make -C $* prog -j $(nproc)


clean:
	$(foreach lib,$(LIBS),make -C $(lib) clean;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) clean;)

compile_flags.txt:
	$(foreach lib,$(LIBS),make -C $(lib) compile_flags.txt;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) compile_flags.txt;)