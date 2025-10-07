include ../../config.mk

SRCS = $(shell find -name '*.fl')

FLFLAGS = --target=bytecode --include=../libwindow/bindings/

prog: $(PROGRAM)

$(PROGRAM): $(SRCS)
ifeq ($(FIRESTORM),1)
	fire compile $(FLFLAGS) --output=../bin/$@ --input=main.fl
endif

clean:
	rm -f $(PROGRAM)

compile_flags.txt: