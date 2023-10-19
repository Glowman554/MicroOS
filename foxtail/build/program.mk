SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin -Ires/libs/include
LDFLAGS = -melf_i386

CFLAGS += $(USER_CFLAGS)

LOAD_ADDR = 0xB0000000

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	i686-linux-gnu-ld $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o $@ $^ $(EXTRA_OBJS) res/libs/libc.o

%.o: %.c
	@echo CC $^
	@i686-linux-gnu-gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) compile_flags.txt

extract_libs:
	unzip -o $(LIBS_ZIP)

compile_flags.txt:
	deno run -A https://raw.githubusercontent.com/Glowman554/MicroOS/master/compile_flags.ts $(CFLAGS) > compile_flags.txt