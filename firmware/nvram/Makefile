TOOLCHAIN_PATH = /opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /usr/local/opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /ef/apps/bin/

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = ravenna_nvram

all: hex lst

hex:  ${PATTERN:=.hex}
lst:  ${PATTERN:=.lst}
sim:  ${PATTERN:=.vcd}

%.vvp: ravenna_nvram_tb.v %.hex
	iverilog -I .. -I ../../../ravenna_ip/source -I ../../../ravenna_ip -o ./ravenna_nvram.vvp $<

%.vcd: ravenna_nvram.vvp
	vvp $<

%.elf: %.c ../sections.lds ../start.s
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -falign-functions=4 -march=rv32imc -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../start.s $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O verilog $< $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O binary $< $@

%.lst: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objdump -d -s $< > $@

flash: ravenna_nvram.hex
	python3 ../../test/ravenna_hkspi.py ravenna_nvram.hex
# 	python3 ../../test/ravenna_flash.py ravenna_nvram.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd *.lst

.PHONY: clean hex all flash

