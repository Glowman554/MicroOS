SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -nostdinc -ffreestanding -no-pie -I include -Wno-builtin-declaration-mismatch -fno-builtin -Ires/libs/include
LDFLAGS = -m32 -ffreestanding -no-pie -nostdlib

CFLAGS += $(USER_CFLAGS)

LOAD_ADDR = 0xB0000000

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	i686-linux-gnu-gcc $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o $@ $^ $(EXTRA_OBJS) res/libs/libc.o -lgcc

%.o: %.c
	@echo CC $^
	@i686-linux-gnu-gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) compile_flags.txt

extract_libs:
	unzip -o $(LIBS_ZIP)

compile_flags.txt:
	deno run -A https://raw.githubusercontent.com/Glowman554/MicroOS/master/compile_flags.ts $(CFLAGS) > compile_flags.txt