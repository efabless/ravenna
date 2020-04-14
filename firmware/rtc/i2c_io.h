#ifndef I2C_IO_H
#define I2C_IO_H
#include "../ravenna_defs.h"

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

#define RTC_I2C_ADDR (uint32_t) 0xA2 // RTC PCF8563
//#define RTC_I2C_ADDR (uint32_t)0xD0 // RTC DS3231

void write_i2c_slave(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t data);
uint32_t read_i2c_slave_byte(volatile uint32_t slave_addr, volatile uint32_t word_addr);
void read_i2c_slave_bytes(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t *data, int len);
void i2c_init(unsigned int pre);
void i2c_start();
void i2c_stop();
bool i2c_write(volatile uint32_t data);
uint32_t i2c_read(bool ack);

int i2c_send(unsigned char saddr, unsigned char sdata);

#endif // I2C_IO_H