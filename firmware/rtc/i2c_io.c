#include "../ravenna_defs.h"
#include "i2c_io.h"

unsigned char volatile * const I2C_PRE_LO = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_PRE_LO_REG);
unsigned char volatile * const I2C_PRE_HI = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_PRE_HI_REG);
unsigned char volatile * const I2C_CTRL = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_CTRL_REG);
unsigned char volatile * const I2C_TX = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_TX_REG);
unsigned char volatile * const I2C_RX = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_RX_REG);
unsigned char volatile * const I2C_CMD = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_CMD_REG);
unsigned char volatile * const I2C_STAT = (unsigned char *) (I2C_BASE_ADDR_0 + I2C_STAT_REG);

#define I2C_PRE_LO      (unsigned char *) (I2C_BASE_ADDR_0 + I2C_PRE_LO_REG);

void i2c_init(unsigned int pre)
{
    reg_i2c_control = 0;
//    *(I2C_PRE_LO) = pre & 0xff;
//    *(I2C_PRE_HI) = pre & 0xff00;
    reg_i2c_prescale = pre;
//    *(I2C_CTRL) = I2C_CTRL_EN | I2C_CTRL_IEN;
//    *(I2C_CTRL) = I2C_CTRL_EN;
    reg_i2c_control = I2C_CTRL_EN;
//    reg_i2c_control = I2C_CTRL_EN | I2C_CTRL_IEN;

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

int write_i2c_slave_byte(volatile uint32_t saddr, volatile uint32_t waddr, volatile uint32_t data)
{
    reg_i2c_data = saddr & 0xff;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = waddr & 0xff;
    reg_i2c_command = I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = data & 0xff;
    reg_i2c_command = I2C_CMD_WR;

    while (reg_i2c_status & I2C_STAT_TIP);
    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
        return 0;
    else
        return 1;

}

int write_i2c_slave_byte_eeprom(volatile uint32_t saddr, volatile uint32_t waddr, volatile uint32_t data)
{
    reg_i2c_data = saddr & 0xff;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = (waddr & 0xff00) >> 8;
    reg_i2c_command = I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = waddr & 0x00ff;
    reg_i2c_command = I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = data;
    reg_i2c_command = I2C_CMD_WR | I2C_CMD_STO;

    while (reg_i2c_status & I2C_STAT_TIP);
//    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
        return 0;
    else
        return 1;

}


uint32_t read_i2c_slave_byte(volatile uint32_t saddr, volatile uint32_t waddr)
{
    // write slave address
    reg_i2c_data = saddr;
    if ((reg_i2c_status & I2C_STAT_TIP) == 1) { reg_gpio_data = 0x0f; return 0; }
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // write word / memory address
    reg_i2c_data = waddr;
    if ((reg_i2c_status & I2C_STAT_TIP) == 1) { reg_gpio_data = 0x0f; return 0; }
    reg_i2c_command = I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // restart and send slave address _ read bit
    reg_i2c_data = saddr | 0x0001;
    if ((reg_i2c_status & I2C_STAT_TIP) == 1) { reg_gpio_data = 0x0f; return 0; }
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // read data
//    *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_NACK | I2C_CMD_STO;
    reg_i2c_command = I2C_CMD_RD | I2C_CMD_STO;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);
//    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1)
        return 0;
    else
        return reg_i2c_data;

}

void i2c_read_data(uint32_t *data)
{
	uint32_t func[&i2c_read_data_begin - &i2c_read_data_end];

	uint32_t *src_ptr = &i2c_read_data_begin;
	uint32_t *dst_ptr = func;

	while (src_ptr != &i2c_read_data_end)
		*(dst_ptr++) = *(src_ptr++);

	((void(*)(uint32_t*))func)(data);
}

int read_i2c_slave_byte_eeprom(volatile uint32_t saddr, volatile uint32_t waddr, volatile uint8_t *data)
{
    // write slave address
    reg_i2c_data = saddr;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // write word / memory address
    reg_i2c_data = (waddr & 0xff00) >> 8 ;
    reg_i2c_command = I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // write word / memory address
    reg_i2c_data = waddr & 0x00ff;
    reg_i2c_command = I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // restart and send slave address _ read bit
    reg_i2c_data = saddr | 0x0001;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;
    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // read data
//    *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_NACK | I2C_CMD_STO;
    i2c_read_data((uint32_t *) data);

    reg_gpio_data = 0x0e;

//    reg_i2c_command = I2C_CMD_RD | I2C_CMD_STO;
//
//    *data = reg_i2c_data & 0xff;
//    while ((reg_i2c_status & I2C_STAT_TIP) != 0) {
//        *data <<= 8;
//        *data |= reg_i2c_data & 0xff;
//    };

//    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1)
        return 0;
    else
        return 1;

}

//uint32_t read_i2c_slave_byte(volatile uint32_t saddr, volatile uint32_t waddr)
//{
////   	volatile uint32_t data;
////
////    i2c_start_write(slave_addr);
//////    if (i2c_start_write(slave_addr << 1))
//////        i2c_stop();
////    i2c_write(word_addr);
////    i2c_start_write(slave_addr | (uint32_t) 0x0001);
////    data = i2c_read(false);
////    i2c_stop();
////
////   	return data;
//
//    *(I2C_TX) = saddr;
//    *(I2C_CMD) = I2C_CMD_STA | I2C_CMD_WR;
//    while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 );
//    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
//        *(I2C_CMD) = I2C_CMD_STO;
//        return 0;
//    }
//
//    // write word / memory address
//    *(I2C_TX) = waddr;
//    *(I2C_CMD) = I2C_CMD_WR;
//    while( (*I2C_STAT) & I2C_STAT_TIP );
//    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
//        *(I2C_CMD) = I2C_CMD_STO;
//        return 0;
//    }
//
//    // restart and send slave address _ read bit
//    *(I2C_TX) = saddr | 0x0001;
//    *(I2C_CMD) = I2C_CMD_STA | I2C_CMD_WR;
//    while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 );
//    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
//        *(I2C_CMD) = I2C_CMD_STO;
//        return 0;
//    }
//
//    // read data
////    *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_NACK | I2C_CMD_STO;
//    *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_STO;
//
//    while( (*I2C_STAT) & I2C_STAT_TIP );
//
//    if( ((*I2C_STAT) & I2C_STAT_RXACK ) == 1)
//        return 0;
//    else
//        return *(I2C_RX);
//}

int read_i2c_slave_bytes(volatile uint32_t saddr, volatile uint32_t waddr, volatile uint32_t *data, int len)
{
//   	int i;
//
//    if (i2c_start_write(slave_addr << 1))
//        i2c_stop();
//    i2c_write(word_addr);
//
//    i2c_write(slave_addr | (uint32_t) 0x0001);  // addr + read mode
//    for (i = 0; i < len-1; i++)
//	    data[i] = i2c_read(true);
//	data[len-1] = i2c_read(false);
//	i2c_stop();

	    // write slave address
    *(I2C_TX) = saddr;
    *(I2C_CMD) = I2C_CMD_STA | I2C_CMD_WR;
    while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 ) {
//        reg_gpio_data = 0x01;
//        reg_gpio_data = (*I2C_STAT) >> 4;
    };
//    reg_gpio_data = 0x02;
    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
        *(I2C_CMD) = I2C_CMD_STO;
        return 0;
    }

    // write word / memory address
    *(I2C_TX) = waddr;
    *(I2C_CMD) = I2C_CMD_WR;
    while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 ) {
//        reg_gpio_data = 0x03;
    };
//    reg_gpio_data = 0x04;
    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
        *(I2C_CMD) = I2C_CMD_STO;
        return 0;
    }

    // restart and send slave address _ read bit
    *(I2C_TX) = saddr | 0x0001;
    *(I2C_CMD) = I2C_CMD_STA | I2C_CMD_WR;
    while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 ) {
//        reg_gpio_data = 0x05;
    };
//    reg_gpio_data = 0x06;
    if( ((*I2C_STAT) & I2C_STAT_RXACK)  == 1) {
        *(I2C_CMD) = I2C_CMD_STO;
        return 0;
    }

    // read data
    for (int i = 0; i < len; i++) {
//        if (i == len-1)
//            *(I2C_CMD) = I2C_CMD_RD;
//        else
//            *(I2C_CMD) = I2C_CMD_RD|I2C_CMD_ACK;

        if (i < len-1) {
            *(I2C_CMD) = I2C_CMD_RD;
        } else {
            *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_STO;
//            *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_NACK | I2C_CMD_STO;
//            *(I2C_CMD) = I2C_CMD_RD | I2C_CMD_NACK;
//            *(I2C_CMD) = I2C_CMD_RD;
        }

        while( ((*I2C_STAT) & I2C_STAT_TIP) != 0 ) {
//            reg_gpio_data = 0x08 | i;
//            reg_gpio_data = 0x08;
    //        reg_gpio_data = (*I2C_STAT) >> 4;
//            reg_gpio_data = (*I2C_STAT);
        };

//        reg_gpio_data = 0x0e;

        if( ((*I2C_STAT) & I2C_STAT_RXACK ) == 1){
            *(I2C_CMD) = I2C_CMD_STO;
            return 0;
        }

        data[i] = *(I2C_RX);
//        data = *(I2C_RX);

    }
//    reg_gpio_data = 0x0e;

//    return *(I2C_RX);
    return 1;


}