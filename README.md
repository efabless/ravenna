# Ravenna

## Programming

To program the Ravenna EVB:

> pip3 install pyftdi
>
> cd firmware/<project>
>
> make flash


## Install Toolchain for Compiling Code

### For Mac

https://github.com/riscv/homebrew-riscv

### For Linux

https://github.com/riscv/riscv-gnu-toolchain

### Using install from PicoRV32

Get compiler toolchain for picorv32…
> % cd ~
>
> % git clone https://github.com/cliffordwolf/picorv32.git
>
> % cd picorv32
>
> % sudo apt-get install autoconf automake autotools-dev curl libmpc-dev \
libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo \
gperf libtool patchutils bc zlib1g-dev git libexpat1-dev
>
> % make download-tools
>
> % make $(nproc) build-riscv32imc-tools
>
> % cd ~/ raven-picorv32/verilog/raven_rtc
>
> % make clean
>
> % make hex
>
> % sudo tclftdi ../../test/startup_flash.tcl
>
> \> write_flash ftdi0 raven_rtc.hex