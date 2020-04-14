#include "../ravenna_defs.h"
#include "i2c_io.h"

void i2c_init(unsigned int pre)
{

    reg_i2c_control = 0;
    reg_i2c_prescale = (uint16_t) pre;
    reg_i2c_control = (uint16_t)(I2C_CTRL_EN);
//    reg_i2c_control = (uint16_t)(I2C_CTRL_EN | I2C_CTRL_IEN);

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
//    reg_i2c_command = I2C_CMD_STA;
//    reg_i2c_command = I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 1;
    }

    reg_i2c_data = sdata;
    reg_i2c_command = I2C_CMD_WR;

    while (reg_i2c_status & I2C_STAT_TIP);
    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
        return 0;
    else
        return 2;
}

void i2c_start()
{
    reg_i2c_command = I2C_CMD_STA;
}

void i2c_stop()
{
    reg_i2c_command = I2C_CMD_STO;
}

void i2c_wait()
{
    while (reg_i2c_status & I2C_STAT_TIP);
}

bool i2c_check_ack()
{
    return reg_i2c_status & I2C_STAT_RXACK;
}

void i2c_send_ack()
{
    reg_i2c_command = I2C_CMD_ACK;
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

    i2c_start_write(slave_addr << 1)
//    if (i2c_start_write(slave_addr << 1))
//        i2c_stop();
    i2c_write(word_addr);
    i2c_start_write(slave_addr << 1 | (uint32_t) 0x0001);
    data = i2c_read(false);
    i2c_stop();

   	return data;
}

void read_i2c_slave_bytes(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t *data, int len)
{
   	int i;

    if (i2c_start_write(slave_addr << 1))
        i2c_stop();
    i2c_write(word_addr);

    i2c_write(slave_addr | (uint32_t) 0x0001);  // addr + read mode
    for (i = 0; i < len-1; i++)
	    data[i] = i2c_read(true);
	data[len-1] = i2c_read(false);
	i2c_stop();

}