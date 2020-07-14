.section .text

.global i2c_inmem_read_begin
.global i2c_inmem_read_end

.balign 4

i2c_inmem_read_begin:
# a0 (in) ... Flags to store in command register
# a1 (in) ... Value for delay fine-tuning
# a0 (out) ... return data (1 byte)

# address of i2c status/cmd reg
li   t0, 0x030000d8  # i2c cmd/stat reg
li   t2, 2
li   t4, 0x030000d4  # i2c control reg
sw   zero, 0(a0)

# write to gpio (this line can be removed safely)
# li   t5, 0x01
# li   t3, 0x03000000  # gpio data reg
# sb  t5, 0(t3)

# Set counter for fine-tuning the timing adjustment
# (see below)
addi  t3, a1, 0

# To ensure the same timing every time this routine is run,
# the i2c must be disabled and reenabled here, which resets
# the clock counter to a known value.  (To do: do a single
# bit clear/set?)
li   t1, 0x40
sb   t1, 2(t4)
li   t1, 0xc0
sb   t1, 2(t4)

# Start read command
# a0 = flags;  0x20 = RD ; 0x68 = RD|ACK|STO
sb   a0, 0(t0)
# Precise timing starts from here!

# Loop at beginning to adjust forward by a specific number of
# cycles.  NOTE:  Middle instruction is important, as either
# no instruction or a noop results in a number of cycles that
# prevents the loop from tuning the position of the "done"
# signal to the correct clock edge of the memory read.

i2c_inmem_read_L1:
addi t3, t3, -1
# andi t2, t2, 0x02
bnez t3, i2c_inmem_read_L1

i2c_inmem_read_L2:
# read data reg (only low byte is non-zero)
lb   a0, 4(t0)

# check for TIP flag (bit 1) & loop if set
lb   t2, 0(t0)
andi t2, t2, 0x02
bnez t2, i2c_inmem_read_L2

ret

.balign 4
i2c_inmem_read_end:


