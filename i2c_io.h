#ifndef I2C_IO_H
#define I2C_IO_H
#include "../ravenna_defs.h"

extern uint32_t i2c_read_data_begin;
extern uint32_t i2c_read_data_end;

#define     I2C_BASE_ADDR_0     0x030000d4

#define     I2C_PRE_LO_REG      0x00
#define     I2C_PRE_HI_REG      0x01
#define     I2C_CTRL_REG        0x02
#define     I2C_TX_REG          0x08
#define     I2C_RX_REG          0x08
#define     I2C_CMD_REG         0x04
#define     I2C_STAT_REG        0x04

#define     I2C_CMD_STA         0x80
#define     I2C_CMD_STO         0x40
#define     I2C_CMD_RD          0x20
#define     I2C_CMD_WR          0x10
#define     I2C_CMD_NACK        0x08
#define     I2C_CMD_IACK        0x01

#define     I2C_CTRL_EN         0x80
#define     I2C_CTRL_IEN        0x40

#define     I2C_STAT_RXACK      0x80
#define     I2C_STAT_BUSY       0x40
#define     I2C_STAT_AL         0x20
#define     I2C_STAT_TIP        0x02
#define     I2C_STAT_IF         0x01

extern unsigned char volatile * const I2C_PRE_LO;
extern unsigned char volatile * const I2C_PRE_HI;
extern unsigned char volatile * const I2C_CTRL;
extern unsigned char volatile * const I2C_TX;
extern unsigned char volatile * const I2C_RX;
extern unsigned char volatile * const I2C_CMD;
extern unsigned char volatile * const I2C_STAT;

//#define RTC_I2C_ADDR (uint32_t) 0xA2 // RTC PCF8563
//#define RTC_I2C_ADDR (uint32_t)0xD0 // RTC DS3231

int write_i2c_slave_byte(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t data);
int write_i2c_slave_byte_eeprom(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t data);
uint32_t read_i2c_slave_byte(volatile uint32_t slave_addr, volatile uint32_t word_addr);
int read_i2c_slave_byte_eeprom(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint8_t *data);
int read_i2c_slave_bytes(volatile uint32_t slave_addr, volatile uint32_t word_addr, volatile uint32_t *data, int len);
void i2c_init(unsigned int pre);
//void i2c_start();
//void i2c_stop();
//bool i2c_write(volatile uint32_t data);
//uint32_t i2c_read(bool ack);

int i2c_send(unsigned char saddr, unsigned char sdata);
void i2c_read_data(uint32_t *data);

#endif // I2C_IO_H