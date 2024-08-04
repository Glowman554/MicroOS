SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -nostdinc -ffreestanding -no-pie -I include -Wno-builtin-declaration-mismatch -fno-builtin -I../libc/include
LDFLAGS = -m32 -ffreestanding -no-pie -nostdlib

LOAD_ADDR = 0xB0000000

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	i686-linux-gnu-gcc $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o ../bin/$@ $^ $(EXTRA_OBJS) ../lib/libc.o -lgcc

%.o: %.c
	@echo CC $^
	@i686-linux-gnu-gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) compile_flags.txt

compile_flags.txt:
	deno run -A ../../compile_flags.ts $(CFLAGS) > compile_flags.txt