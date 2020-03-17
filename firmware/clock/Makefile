TOOLCHAIN_PATH = /opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /ef/apps/bin/

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = clock

all: hex client

hex:  ${PATTERN:=.hex}

%.elf: %.c ../sections.lds ../start.s
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -march=rv32imc -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../start.s $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O verilog $< $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: clock.hex
# 	python3 ../../test/ravenna_flash.py clock.hex
	python3 ../../test/ravenna_hkspi.py clock.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all flash

