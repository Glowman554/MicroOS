SRCS = $(shell find -name '*.[cS]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

OPT_LVL = 2

CFLAGS = -O$(OPT_LVL) -m32 -Wall -g -fno-pic -fno-pie -fno-stack-protector -nostdinc -I include -Wno-builtin-declaration-mismatch -fno-builtin -Ires/libs/include
LDFLAGS = -melf_i386
ARFLAGS = rcs

CFLAGS += $(USER_CFLAGS)

CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld
AR = i686-linux-gnu-ar

LIBRARY_A = $(addsuffix .a,$(basename $(LIBRARY)))

library: $(LIBRARY) $(LIBRARY_A)

$(LIBRARY): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -r -o ./$@ $^ $(EXTRA_OBJS)

$(LIBRARY_A): $(OBJS)
	@echo AR $^
	@$(AR) $(ARFLAGS) ./$@ $^

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(LIBRARY) $(LIBRARY_A) compile_flags.txt

extract_libs:
	unzip -o $(LIBS_ZIP)

compile_flags.txt:
	@compile-flags $(CFLAGS) > compile_flags.txt