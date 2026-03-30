include ../../config.mk

SRCS = $(shell find -name '*.fl')

FLFLAGS = --target=bytecode

prog: $(PROGRAM)

$(PROGRAM): $(SRCS)
ifeq ($(FIRESTORM),1)
	@echo FL $^
	@fire compile $(FLFLAGS) --output=../bin/$@ --input=main.fl
endif

clean:
	rm -f $(PROGRAM)

compile_flags.txt: