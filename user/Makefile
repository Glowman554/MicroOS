include ../config.mk

LIBS = libraries/libc libraries/libtinf libraries/libflvm libraries/libjson \
	   libraries/libload libraries/libsyntax libraries/libdns

PROGRAMS = base/background base/calc base/cat base/clear base/cp base/date \
           base/du base/echo base/edit base/env base/figl base/file base/free \
		   base/if base/init base/install base/linux base/lminst base/lolcat \
		   base/ls base/mkdir base/pciinfo base/play base/pwr base/rm base/rmdir \
		   base/tasks base/terminal base/test base/touch base/tree base/wc

PROGRAMS += development/createmex development/fasm development/flvm development/highlight \
            development/jsonprint development/link development/load development/mcc

PROGRAMS += network/dhcp network/httpget network/ip network/listen network/nslookup network/ping \
		    network/stream

PROGRAMS += services/resolved services/scheduler services/service services/shortcutd

ifeq ($(GUI),1)
	LIBS += desktop/libdesktop

	PROGRAMS += desktop/desktop desktop/counter desktop/sysctl desktop/taskmgr \
	            desktop/netinfo desktop/explorer desktop/imgview desktop/service
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
	make -C $* deploy SYSROOT=../../../.cross/i686-microos

%_prog:
	mkdir -p bin
	make -C $* prog


clean:
	$(foreach lib,$(LIBS),make -C $(lib) clean;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) clean;)

compile_flags.txt:
	$(foreach lib,$(LIBS),make -C $(lib) compile_flags.txt;)
	$(foreach prog,$(PROGRAMS),make -C $(prog) compile_flags.txt;)