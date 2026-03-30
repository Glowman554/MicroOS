SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-stack-protector -nostdinc -ffreestanding -no-pie -I include -Wno-builtin-declaration-mismatch -fno-builtin -Ires/libs/include
LDFLAGS = -m32 -ffreestanding -no-pie -nostdlib -L./res/libs

CFLAGS += $(USER_CFLAGS)

LOAD_ADDR = 0xB0000000

CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-gcc

prog: $(PROGRAM)

$(PROGRAM): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -Ttext=$(LOAD_ADDR) -o $@ $^ $(EXTRA_OBJS) $(LINK) -lc -lgcc
	@createmex -a $(AUTHOR) -b 6 $@ $(addsuffix .mex,$(basename $@))

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM) $(addsuffix .mex,$(basename $(PROGRAM))) compile_flags.txt

extract_libs:
	unzip -o $(LIBS_ZIP)

compile_flags.txt:
	@compile-flags $(CFLAGS) > compile_flags.txt