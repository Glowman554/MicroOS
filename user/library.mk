SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin
LDFLAGS = -melf_i386

library: $(LIBRARY)

$(LIBRARY): $(OBJS)
	ld $(LDFLAGS) -r -o ../lib/$@ $^

%.o: %.c
	@echo CC $^
	@gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY) compile_flags.txt

compile_flags.txt:
	deno run -A ../../compile_flags.ts $(CFLAGS) > compile_flags.txt