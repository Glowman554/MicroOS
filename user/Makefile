include ../config.mk

LIBS = libc libtinf libflvm libjson libload libsyntax libdns
PROGRAMS = init terminal ls cat test echo calc pwr edit clear \
           ping nslookup play wc cp env file mkdir rm rmdir date background \
		   fasm figl du linux tasks stream free install lminst touch dhcp ip \
		   pciinfo flvm scheduler if httpget jsonprint createmex lolcat load \
		   tree mcc highlight resolved

ifeq ($(GUI),1)
	PROGRAMS += desktop
endif

all:
	make build_libraries
	make build_programs

build_libraries: $(addsuffix _library,$(LIBS))
deploy_libraries: $(addsuffix _library_deploy,$(LIBS))
build_programs: $(addsuffix _prog,$(PROGRAMS))

%_library:
	mkdir -p lib
	make -C $* library

%_library_deploy:
	mkdir -p lib
	make -C $* deploy SYSROOT=../../.cross/i686-microos

%_prog:
	mkdir -p bin
	make -C $* prog


clean:
	$(foreach lib,$(LIBS),make -C $(lib) clean;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) clean;)

compile_flags.txt:
	$(foreach lib,$(LIBS),make -C $(lib) compile_flags.txt;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) compile_flags.txt;)