
CORE = core
MODULES = network/am79C973 network/e1000 network/ne2k network/rtl8139 network/vlan \
          filesystem/fat32 filesystem/ramfs \
          feature/acpi_dump feature/lai feature/system \
		  driver/ac97 driver/ata driver/ps2 driver/ahci

all: _core $(addsuffix _module,$(MODULES))

_core:
	make -C $(CORE)

%_module:
	mkdir -p modules
	make -C $(patsubst %_module,%,$@)


clean:
	make -C $(CORE) clean
	$(foreach mod,$(MODULES),make -C $(mod) clean;)


compile_flags.txt:
	make -C $(CORE) compile_flags.txt
	$(foreach mod,$(MODULES),make -C $(mod) compile_flags.txt;)