TEST = test.o

CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch
LDFLAGS = -melf_i386

all: test.elf

test.elf: $(TEST)
	ld $(LDFLAGS) -Ttext=0x800000 -o $@ $^

install:
	cp *.elf ../cdrom/bin/ -v

%.o: %.c
	@echo CC $^
	@gcc $(CFLAGS) -c -o $@ $^

clean:
	rm *.o
	rm *.elf