include ../../../config.mk

SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

ASFLAGS = -m32 -g
CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -nostdinc -ffreestanding -no-pie -I include -Wno-builtin-declaration-mismatch -fno-builtin -I../../libraries/libc/include
LDFLAGS = -m32 -ffreestanding -no-pie -nostdlib -L../../lib 

LOAD_ADDR = 0xB0000000

CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-gcc

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o ../../bin/$@ $^ $(EXTRA_OBJS) $(LINK) -lc -lgcc
	@../../../res/createmex -a glowman554 -b $(ABI_VERSION) ../../bin/$@ ../../bin/$(addsuffix .mex,$(basename $@))

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	@echo AS $^
	@$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) compile_flags.txt

compile_flags.txt:
	@../../../res/compile-flags $(CFLAGS) > compile_flags.txt