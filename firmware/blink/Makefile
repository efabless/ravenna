TOOLCHAIN_PATH = /opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /ef/apps/bin/

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = blink

all:  ${PATTERN:=.vcd}

hex:  ${PATTERN:=.hex}

%.elf: %.c sections.lds start.s i2c_io.c i2c_io.h spi_io.c spi_io.h cam.c cam.h
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -march=rv32imc -Wl,-Bstatic,-T,sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ start.s i2c_io.c spi_io.c cam.c $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O verilog $< $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all

