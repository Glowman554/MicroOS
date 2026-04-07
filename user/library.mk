SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-pic -fno-pie -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin
LDFLAGS = -melf_i386
ARFLAGS = rcs

CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld
AR = i686-linux-gnu-ar

LIBRARY_A = $(addsuffix .a,$(basename $(LIBRARY)))

library: $(LIBRARY) $(LIBRARY_A)

$(LIBRARY): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -r -o ../../lib/$@ $^ $(EXTRA_OBJS)

$(LIBRARY_A): $(OBJS)
	@echo AR $^
	@$(AR) $(ARFLAGS) ../../lib/$@ $^

deploy: $(LIBRARY_A)
ifdef SYSROOT
	mkdir -p $(SYSROOT)/lib
	mkdir -p $(SYSROOT)/include
	cp ../../lib/$(LIBRARY_A) $(SYSROOT)/lib/
	cp include/* $(SYSROOT)/include/. -rf
else
	$(error "No SYSROOT specified.")
endif

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY) compile_flags.txt

compile_flags.txt:
	@../../../res/compile-flags $(CFLAGS) > compile_flags.txt