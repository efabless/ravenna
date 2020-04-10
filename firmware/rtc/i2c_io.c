#include "../ravenna_defs.h"
#include "i2c_io.h"

// command register bits
// bit 7 = start
// bit 6 = stop
// bit 5 = read
// bit 4 = write

// control register bits
// bit 27 = acknowledge
// bit 24 = interrupt acknowledge
// bit 23 = enable
// bit 22 = interrupt enable

// bits 15-0:  clock prescaler

#define     I2C_CMD_STA         0x80
#define     I2C_CMD_STO         0x40
#define     I2C_CMD_RD          0x20
#define     I2C_CMD_WR          0x10
#define     I2C_CMD_ACK         0x08
#define     I2C_CMD_IACK        0x01

#define     I2C_CTRL_EN         0x80
#define     I2C_CTRL_IEN        0x40

// status regiter bits:
// bit 7 = receive acknowledge
// bit 6 = busy (start signal detected)
// bit 5 = arbitration lost
// bit 1 = transfer in progress
// bit 0 = interrupt flag

#define     I2C_STAT_RXACK      0x80
#define     I2C_STAT_BUSY       0x40
#define     I2C_STAT_AL         0x20
#define     I2C_STAT_TIP        0x02
#define     I2C_STAT_IF         0x01

-----

#define I2C_START           0x80
#define I2C_START_WRITE     0x90
#define I2C_STOP            0x40
#define I2C_READ            0x20
#define I2C_WRITE           0x10
#define I2C_BUSY            0x02
#define I2C_CHECK_ACK       0x80
#define I2C_SEND_ACK        0x08

void i2c_init(unsigned int pre)
{

    reg_i2c_control = (uint16_t)(I2C_CTRL_EN | I2C_CTRL_IEN);
    reg_i2c_prescale = (uint16_t) pre;

    // enable (bit 23)
    // clock divider 0x0280 = 100kb/s (standard mode)
    // clock divider 0x00a0 = 100kb/s (standard mode)
//    reg_i2c_config = 0x000000a0;
//    reg_i2c_config = 0x008000a0;
//    reg_i2c_control = ;
//    reg_i2c_prescale = ;
}

int i2c_send(unsigned char saddr, unsigned char sdata) {

    int volatile y;
    reg_i2c_data = saddr;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = sdata;
    reg_i2c_command = I2C_CMD_WR;

    while (reg_i2c_status & I2C_STAT_TIP);
    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
        return 0;
    else
        return 1;
}

void i2c_stop()
{
    reg_i2c_command = I2C_STOP;
}

void i2c_wait()
{
    while (reg_i2c_status & I2C_BUSY) {}
}

bool i2c_check_ack()
{
    return reg_i2c_status & I2C_CHECK_ACK;
}

void i2c_send_ack()
{
    reg_i2c_command = I2C_SEND_ACK;
}

bool i2c_start_write(volatile uint32_t data)
{
    reg_i2c_data = data;
    reg_i2c_command = I2C_START_WRITE;
    i2c_wait();
    return i2c_check_ack();
}

bool i2c_write(volatile uint32_t data)
{

    reg_i2c_data = data;
    reg_i2c_command = I2C_WRITE;
    i2c_wait();
    return i2c_check_ack();

}

volatile uint32_t i2c_read(bool ack)
{
	volatile uint32_t data;

    reg_i2c_command = I2C_READ;
    i2c_wait();
    data = reg_i2c_data;
	if (ack)
	    i2c_send_ack();

   return data;
}

void write_i2c_slave(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t data)
{
    if (i2c_start_write(slave_addr))
        i2c_stop();

    i2c_write(word_addr);
    i2c_write(data);

    i2c_stop();

}

uint32_t read_i2c_slave_byte(volatile uint32_t slave_addr, volatile uint32_t word_addr)
{
   	volatile uint32_t data;

    if (i2c_write(slave_addr))
        i2c_stop();
    i2c_write(word_addr);
    data = i2c_read(false);

    i2c_stop();

   	return data;
}

void read_i2c_slave_bytes(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t *data, int len)
{
   	int i;

    if (i2c_write(slave_addr))
        i2c_stop();
    i2c_write(word_addr);

    i2c_write(slave_addr | (uint32_t) 0x0001);  // addr + read mode
    for (i = 0; i < len-1; i++)
	    data[i] = i2c_read(true);
	data[len-1] = i2c_read(false);
	i2c_stop();

}