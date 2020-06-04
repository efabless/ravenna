.section .text

.global i2c_read_data_begin
.global i2c_read_data_end

.balign 4

i2c_read_data_begin:
# a0 ... return data

# address of i2c status/cmd reg
li   t0, 0x030000d8  # i2c cmd/stat reg
li   t3, 0x03000000  # gpio data reg
li   t2, 2
li   t4, 610
li   t5, 0x01
sw   zero, 0(a0)

# Start read command
li   t1, 0x60
sb   t1, 0(t0)
# lb   a0, 4(t0)

# loop delay
i2c_read_data_L1:
addi t4, t4, -1
bnez t4, i2c_read_data_L1
#sb  t2, 0(t3)
# addi x0, x0, 1
#sb  t2, 0(t3)
nop
nop
#add x0, x1, x2

# write to gpio
sb  t5, 0(t3)

.balign 4
i2c_read_data_L2:
# read data reg & mask off 1 byte
lb   a0, 4(t0)
#andi t1, t1, 0xff

# shift data register + OR new byte
#slli a0, a0, 8
#or  a0, a0, t1

# check for TIP flag (bit 1) & loop if set
# lb   t2, 0(t0)
# andi t2, t2, 0x02
# bnez t2, i2c_read_data_L2

ret

.balign 4
i2c_read_data_end:



