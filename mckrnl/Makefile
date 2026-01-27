
CORE = core
MODULES = network/am79C973 network/e1000 network/ne2k network/rtl8139 \
          filesystem/fat32 filesystem/nextfs filesystem/ramfs \
          feature/acpi_dump feature/lai \
		  driver/ac97 driver/ata driver/ps2

all: _core $(addsuffix _module,$(MODULES))

_core:
	make -C $(CORE) -j $(nproc)

%_module:
	mkdir -p modules
	make -C $(patsubst %_module,%,$@) -j $(nproc)


clean:
	make -C $(CORE) clean
	$(foreach mod,$(MODULES),make -C $(mod) clean;)


compile_flags.txt:
	make -C $(CORE) compile_flags.txt
	$(foreach mod,$(MODULES),make -C $(mod) compile_flags.txt;)