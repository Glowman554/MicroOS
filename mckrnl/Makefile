SRCS = $(shell find -name '*.[cS]')
CPPSRC = $(shell find -name '*.cpp')
OBJS = $(addsuffix .o,$(basename $(SRCS) $(CPPSRC)))
RUSTSRC = $(shell find -name '*.rs')

CC = gcc
LD = ld

ASFLAGS = -m32
CFLAGS = -fsanitize=undefined -m32 -Wall -g -fno-stack-protector -Wno-builtin-declaration-mismatch -fno-builtin -nostdinc -Iinclude -Wno-nonnull-compare -Wno-main
LDFLAGS = -melf_i386 -Tlink.ld

mckrnl.elf: $(OBJS) target/i686-unknown-microos/debug/libkernel.a
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $@ $^

target/i686-unknown-microos/debug/libkernel.a: $(RUSTSRC)
	cargo build

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	@echo AS $^
	@$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)
	rm -rf target

.PHONY: clean
