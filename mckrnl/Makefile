SRCS = $(shell find -name '*.[cS]')
CPPSRC = $(shell find -name '*.cpp')
OBJS = $(addsuffix .o,$(basename $(SRCS) $(CPPSRC)))

CC = gcc
LD = ld

OPT_LVL = 2

ASFLAGS = -m32
CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -Wno-builtin-declaration-mismatch -fno-builtin -nostdinc -Iinclude -Wno-nonnull-compare -Wno-main
LDFLAGS = -melf_i386 -Tlink.ld

ifeq ($(SANITIZE), true)
	CFLAGS += -fsanitize=undefined -DSANITIZE
endif

mckrnl.elf: $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.S
	@echo AS $^
	@$(CC) $(ASFLAGS) -c -o $@ $^

clean:
	rm $(OBJS)

.PHONY: clean
