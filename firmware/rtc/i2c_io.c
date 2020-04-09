#include "../ravenna_defs.h"
#include "i2c_io.h"

#define SDA_PIN (uint32_t) (1 << 14) // bit 14
#define SCL_PIN (uint32_t) (1 << 15) // bit 15

#define SCL_OUT (volatile uint32_t) ((reg_gpio_ena) &= ~(SCL_PIN))
#define SCL_IN (volatile uint32_t) (reg_gpio_ena |= (SCL_PIN))
#define SDA_OUT (volatile uint32_t) ((reg_gpio_ena) &= ~(SDA_PIN))
#define SDA_IN (volatile uint32_t) (reg_gpio_ena |= (SDA_PIN))

#define SCL_HIGH SCL_IN
#define SCL_LOW SCL_OUT; (volatile uint32_t) ((reg_gpio_data) &= ~(SCL_PIN))
#define SCL_READ (volatile uint32_t) ((reg_gpio_data) & (SCL_PIN))
#define SDA_HIGH SDA_IN
#define SDA_LOW SDA_OUT; (volatile uint32_t) ((reg_gpio_data) &= ~(SDA_PIN))
#define SDA_READ (volatile uint32_t) ((reg_gpio_data) & (SDA_PIN))

#define I2C_START           0x80
#define I2C_START_WRITE     0x90
#define I2C_STOP            0x40
#define I2C_READ            0x20
#define I2C_WRITE           0x10
#define I2C_BUSY            0x02
#define I2C_CHECK_ACK       0x80
#define I2C_SEND_ACK        0x08

void i2c_delay()
{

//  I2C standard mode (100k) = 5 usec min hold time

//	for (int j = 0; j < 200000; j++);  // 1 secs
//	for (int j = 0; j < 100000; j++);  // 0.5 secs
	for (int j = 0; j < 1; j++);  // ~23 usec (measured)

}

void i2c_init()
{
    // enable (bit 23)
    // clock divider 0x0280 = 100kb/s (standard mode)
    reg_i2c_config = 0x00000280;
    reg_i2c_config = 0x00800280;
}

void i2c_start_write()
{
    reg_i2c_command = I2C_START_WRITE;
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
    return reg_i2c_status & I2C_ACK;
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
    return i2c_check_ack()
}

bool i2c_write(volatile uint32_t data)
{

    reg_i2c_data = slave_addr;
    reg_i2c_command = I2C_WRITE;
    i2c_wait();
    return i2c_check_ack()

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
    data = i2c_read();

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