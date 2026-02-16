include ../../config.mk

SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

ASFLAGS = -m32 -g
CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -nostdinc -ffreestanding -no-pie -I include -Wno-builtin-declaration-mismatch -fno-builtin -I../libc/include
LDFLAGS = -m32 -ffreestanding -no-pie -nostdlib

LOAD_ADDR = 0xB0000000

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	i686-linux-gnu-gcc $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o ../bin/$@ $^ $(EXTRA_OBJS) ../lib/libc.o -lgcc
	@deno run -A ../../encode_mex_v2.ts glowman554 $(ABI_VERSION) ../bin/$@ ../bin/$(addsuffix .mex,$(basename $@))

%.o: %.c
	@echo CC $^
	@i686-linux-gnu-gcc $(CFLAGS) -c -o $@ $^

%.o: %.S
	@echo AS $^
	@$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) compile_flags.txt

compile_flags.txt:
	deno run -A ../../compile_flags.ts $(CFLAGS) > compile_flags.txt