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
  
  To compile and download the firmware example code, first connect Ravenna 
  to your workstation using the USB cable.  No jumper should be installed on the board.
  
> % cd ravenna/firmware/blink
>
> % make clean
>
> % make hex
>
> % make flash
