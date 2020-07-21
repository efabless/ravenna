#include "../ravenna_defs.h"
#include "../ravenna_io.h"
//#include "spi.h"

void gpio_set_dir(unsigned int dir){
    reg_gpio_enb = dir;
}

unsigned int gpio_read(){
    return reg_gpio_data;
}

void gpio_write(unsigned int data){
    reg_gpio_data = data;
}

void spi_start_transaction(){
//  SET_BIT(*SPI_CTRL, SPI_SS_BIT);
  reg_spi_config = 32 + 0x3000;

}

void spi_finish_transaction(){
  reg_spi_config = 32 + 0x2000;

}

void spi_configure(unsigned char cpol, unsigned char cpha, unsigned char clkdiv){
//  unsigned int cfg_value = ( 1 << 13);  // enable
  unsigned int cfg_value = ( 0x2000 );  // enable
//  cfg_value |=  (cpol << 0);
//  cfg_value |=  (cpha << 1);
  cfg_value |=  ((unsigned int) clkdiv & 0xff);
//  *SPI_CFG = cfg_value;
//  reg_spi_config = cfg_value;
  reg_spi_config = 1 + 0x2000;
}

unsigned char spi_read(){
  return reg_spi_data & 0xff;
}
void spi_write(unsigned char data){
    reg_spi_data =  data & 0xff;
}

void write_byte(unsigned int addr, unsigned int data){
//  spi_start_transaction();
//  spi_write(0x2);
//  spi_write(addr >> 8);     // Address high byte
  spi_write(addr & 0xFF);   // Address low byte
  spi_write(data);
//  spi_finish_transaction();
}

unsigned char read_byte(unsigned short addr){
    unsigned char data;
//  spi_start_transaction();
//  spi_write(0x00);
//  spi_write(addr >> 8);     // Address high byte
  spi_write(addr & 0xFF);   // Address low byte
  spi_write(0);             // just write a dummy data to get the data out
//  data = spi_read();
//  spi_write(0);             // just write a dummy data to get the data out
//  spi_write(0);             // just write a dummy data to get the data out
//  spi_finish_transaction();
  return spi_read();
//  return data;
}

unsigned char check_cam(){
    unsigned char data;
//  spi_start_transaction();
  spi_write(0x55);     // Address high byte
  spi_write(0x00);
  spi_write(0);             // just write a dummy data to get the data out
  data = spi_read();
//  spi_finish_transaction();
  return data;
}

//unsigned char read_byte(unsigned short addr){
//  spi_start_transaction();
//  spi_write(0x3);
//  spi_write(addr >> 8);     // Address high byte
//  spi_write(addr & 0xFF);   // Address low byte
//  spi_write(0);             // just write a dummy data to get the data out
//  spi_finish_transaction();
//  return spi_read();
//}

void main()
{
//	uint32_t i, j;
	int i, j;
	int x, v;
    unsigned char data[2];

//    set_clock(true, true, 0);

    // Enable GPIO (all output, ena = 0)
	gpio_set_dir(0x0000);
    gpio_write(0x000f);
//
//    for (j = 0; j < 70000; j++);

//    print("Starting...");
//    spi_configure(0,0,20);

//    write_byte(0x80, 0x55);
//    data[0] = read_byte(0x00);

  spi_start_transaction();
//  reg_spi_config = 1 + 0x3000;

//  spi_write(0x2);
//  spi_write(addr >> 8);     // Address high byte
//  spi_write(0x80);   // Address low byte
  spi_write(0x80);   // Address low byte
//  reg_spi_data = 0xab;
//  reg_spi_data = 0x00;
  spi_write(0x55);
//  reg_spi_data = 0x55;

//  reg_spi_config = 1 + 0x2000;
  spi_finish_transaction();

  for (j = 0; j < 2; j++);

  spi_start_transaction();
//  reg_spi_config = 1 + 0x3000;
//
////  spi_write(0x00);
////  spi_write(addr >> 8);     // Address high byte
  spi_write(0x00);   // Address low byte
  spi_write(0);             // just write a dummy data to get the data out
////  spi_finish_transaction();
  data[0] = spi_read();
//
////  reg_spi_config = 1 + 0x2000;
  spi_finish_transaction();


//    data[0] = check_cam();
//    print_hex(data[0], 2); print(":");
    if (data[0] == 0x55)
        gpio_write(0x0a);
    else
        gpio_write(0x01);
//    gpio_write(data[0] >> 4);
//        print_hex(data[1], 2); print(":");
//        print_hex(data[2], 2);
//        } else {
//            gpio_write(0x09);
//        }
//        for (j = 0; j < 34000; j++) {} // 2 sec
//        gpio_write(0x0005);
//    for (j = 0; j < 34000; j++) {} // 2 sec
//    }

    while (1);
}

