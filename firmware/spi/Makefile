TOOLCHAIN_PATH = /opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /ef/apps/bin/

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = spi ravenna_spi

all: hex

hex:  ${PATTERN:=.hex}
lst:  ${PATTERN:=.lst}
sim:  ${PATTERN:=.vcd}

%.vvp: spi_tb.v %.hex
	iverilog -I .. -I ../../../ravenna_ip/source -I ../../../ravenna_ip -o ./spi.vvp $<

%.vcd: spi.vvp
	vvp $<

%.elf: %.c ../sections.lds ../start.s ../ravenna_defs.h ../ravenna_io.h
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -march=rv32imc -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../start.s ../ravenna_io.c $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O verilog $< $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: spi.hex
	python3 ../../test/ravenna_hkspi.py spi.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all flash

