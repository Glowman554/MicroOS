SRCS = $(shell find -name '*.[c]')
OBJS = $(addsuffix .o,$(basename $(SRCS)))


CFLAGS = -O2 -Wall -Iinclude -g
LDFLAGS = -lunicorn
PROGRAM ?= microemu

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	cp $(PROGRAM) ~/go/bin/$(PROGRAM)

%.o: %.c
	@echo CC $^
	@$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJS) $(PROGRAM)

