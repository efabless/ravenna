TOOLCHAIN_PATH = /opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /usr/local/opt/riscv32imc/bin/
# TOOLCHAIN_PATH = /ef/apps/bin/

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = rtc

all: hex lst client

hex:  ${PATTERN:=.hex}
lst:  ${PATTERN:=.lst}
sim:  ${PATTERN:=.vcd}

%.vvp: rtc_tb.v %.hex
	iverilog -I .. -I ../../../ravenna_ip/source -I ../../../ravenna_ip -o ./rtc.vvp $<

%.vcd: rtc.vvp
	vvp $<

%.elf: %.c ../sections.lds ../start.s ../i2c_read.s
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-gcc -falign-functions=4 -march=rv32imc -Wl,-Bstatic,-T,../sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ ../start.s ../i2c_read.s $<

%.hex: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O verilog $< $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objcopy -O binary $< $@

%.lst: %.elf
	$(TOOLCHAIN_PATH)riscv32-unknown-elf-objdump -d -s $< > $@

client: client.c
	gcc client.c -o client

flash: rtc.hex
	python3 ../../test/ravenna_hkspi.py rtc.hex
# 	python3 ../../test/ravenna_flash.py rtc.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd *.lst

.PHONY: clean hex all flash

