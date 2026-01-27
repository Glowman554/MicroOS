MODULE = lai.o

all: lai
	make $(MODULE)

lai:
	git clone https://github.com/managarm/lai.git

include ../../module.mk

CFLAGS += -Ilai/include -Wno-format