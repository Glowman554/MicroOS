SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -m32 -Wall -g -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin -I../libc/include
LDFLAGS = -melf_i386

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	ld $(LDFLAGS) -Ttext=0xA0000000 -o ../bin/$@ $^ ../lib/libc.o

%.o: %.c
	@echo CC $^
	@gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY)