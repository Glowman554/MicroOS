SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-pic -fno-pie -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin -Ires/libs/include
LDFLAGS = -melf_i386

CFLAGS += $(USER_CFLAGS)

library: $(LIBRARY)

$(LIBRARY): $(OBJS)
	i686-linux-gnu-ld $(LDFLAGS) -r -o ./$@ $^ $(EXTRA_OBJS)

%.o: %.c
	@echo CC $^
	@i686-linux-gnu-gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY) compile_flags.txt

extract_libs:
	unzip -o $(LIBS_ZIP)

compile_flags.txt:
	deno run -A https://raw.githubusercontent.com/Glowman554/MicroOS/master/compile_flags.ts $(CFLAGS) > compile_flags.txt