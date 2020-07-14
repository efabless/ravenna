#include "../ravenna_defs.h"

/* ravenna_i2c.c:  Implements in-memory read to work around the	*/
/* i2c module bug.  Allows multiple byte reads.  In-memory	*/
/* function is read from program memory at the top of main()	*/	
/* and not again.  This is the "uglier" version because the	*/
/* memory for func() is allocated in the main routine and	*/
/* passed to the i2c_receive() subroutine, but it is the most	*/
/* efficient version.						*/

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
#define RTC_I2C_ADDR (uint32_t) 0xA2 // RTC PCF8563
#define RTC_I2C_REG (uint32_t) 0x02 // RTC PCF8563

#define BCD_DIGIT0(x) (x & (uint32_t)0x000F)
#define BCD_DIGIT1(x) ((x >> 4) & (uint32_t)0x000F)

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

extern uint32_t i2c_inmem_read_begin;
extern uint32_t i2c_inmem_read_end;

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	reg_uart_data = c & 0xff;
}

void print(const char *p)
{
	while (*p)
		putchar(*(p++));
}

void clear()
{
	reg_uart_data = 0x7c;
	reg_uart_data = 0x2d;
}


void print_hex(uint32_t v, int digits)
{
	for (int i = digits - 1; i >= 0; i--) {
		char c = "0123456789abcdef"[(v >> (4*i)) & 15];
		putchar(c);
	}
}


void print_digit(uint32_t v)
{
	v &= (uint32_t)0x000F;

	if (v == 9) { putchar('9'); }
	else if (v == 8) { putchar('8'); }
	else if (v == 7) { putchar('7'); }
	else if (v == 6) { putchar('6'); }
	else if (v == 5) { putchar('5'); }
	else if (v == 4) { putchar('4'); }
	else if (v == 3) { putchar('3'); }
	else if (v == 2) { putchar('2'); }
	else if (v == 1) { putchar('1'); }
	else if (v == 0) { putchar('0'); }
	else putchar('*');
}

// --------------------------------------------------------

void i2c_init(unsigned int pre) {

    reg_i2c_control = (uint16_t)(I2C_CTRL_EN | I2C_CTRL_IEN);
    reg_i2c_prescale = (uint16_t)pre;
}

// --------------------------------------------------------
// Send one or more bytes over i2c
// --------------------------------------------------------

int i2c_send(unsigned char saddr, unsigned char waddr, unsigned char *sdata, int len) {
    int i;

    reg_i2c_data = saddr;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }
    reg_i2c_data = waddr;
    reg_i2c_command = I2C_CMD_WR;

    while (reg_i2c_status & I2C_STAT_TIP);

    for (i = 0; i < len; i++)
    {
    	reg_i2c_data = *(sdata + i);
    	reg_i2c_command = I2C_CMD_WR;

    	while (reg_i2c_status & I2C_STAT_TIP);
    }
    reg_i2c_command = I2C_CMD_STO;

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
        return 0;
    else
        return 1;
}

// --------------------------------------------------------

void i2c_inmem_load(uint32_t *func)
{
    uint32_t *src_ptr = &i2c_inmem_read_begin;
    uint32_t *dst_ptr = func;

    while (src_ptr != &i2c_inmem_read_end)
        *(dst_ptr++) = *(src_ptr++);
}

// --------------------------------------------------------
// Read performed in SRAM for speed, so that the read can
// be aligned with the short "done" pulse.  (1) Copy the
// routine from flash memory to SRAM, (2) launch the read
// from SRAM.
// --------------------------------------------------------

uint32_t i2c_inmem_read(uint32_t flags, uint32_t adjust, uint32_t *func)
{
    return ((uint32_t(*)(uint32_t, uint32_t))func)(flags, adjust);
}

// --------------------------------------------------------
// Receive one or more bytes over i2c
// --------------------------------------------------------

int i2c_receive(unsigned char saddr, unsigned char waddr,
	unsigned char *sdata, int len, uint32_t *func) {
    int i;
    uint32_t rbyte;

    reg_i2c_data = saddr;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    reg_i2c_data = waddr;
    reg_i2c_command = I2C_CMD_WR;

    while ((reg_i2c_status & I2C_STAT_TIP) != 0);

    if ((reg_i2c_status & I2C_STAT_RXACK)  == 1) {
        reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    // Restart and send slave address + read bit

    reg_i2c_data = saddr | 0x0001;
    reg_i2c_command = I2C_CMD_STA | I2C_CMD_WR;

    while (reg_i2c_status & I2C_STAT_TIP);

    if ((reg_i2c_status & I2C_STAT_RXACK) == 1)
    {
    	reg_i2c_command = I2C_CMD_STO;
        return 0;
    }

    for (i = 0; i < len; i++) {
	if (i == len - 1)
	    rbyte = i2c_inmem_read(I2C_CMD_RD | I2C_CMD_ACK | I2C_CMD_STO, 0x0d, func);
	else
	    rbyte = i2c_inmem_read(I2C_CMD_RD, 0x19, func);

        *(sdata + i) = (uint8_t)rbyte & 0xff;
    }
    return 1;
}

// --------------------------------------------------------
// Read time value from the RTC module starting at
// register 0x2 (RTC_I2C_REG).
// --------------------------------------------------------

void read_rtc(uint32_t *func)
{	
    uint8_t data[3];
    uint8_t data_0;
    uint8_t data_1;
    uint8_t data_2;

    data[0] = data[1] = data[2] = 0;
    data_0 = data_1 = data_2 = 0;

//    rtc_stop();

//    i2c_receive(RTC_I2C_ADDR, RTC_I2C_REG, data, 3, func);
    i2c_receive(RTC_I2C_ADDR, RTC_I2C_REG, &data_0, 1, func);
    i2c_receive(RTC_I2C_ADDR, RTC_I2C_REG+1, &data_1, 1, func);
    i2c_receive(RTC_I2C_ADDR, RTC_I2C_REG+2, &data_2, 1, func);


//    rtc_run();

    data[0] &= (uint8_t) 0x7F;
    data[1] &= (uint8_t) 0x7F;
    data[2] &= (uint8_t) 0x3F;

    data_0 &= (uint8_t) 0x7F;
    data_1 &= (uint8_t) 0x7F;
    data_2 &= (uint8_t) 0x3F;

    // Print time in HH:MM:SS

//    clear();
    print("\r");
    print("Time = ");
//    print_digit(BCD_DIGIT1(data[2]));
    print_digit(BCD_DIGIT1(data_2));
//    print_digit(BCD_DIGIT0(data[2]));
    print_digit(BCD_DIGIT0(data_2));
//    print_hex(data[2],2);
//    print_hex(data_2,2);
    print(":");
//    print_digit(BCD_DIGIT1(data[1]));
    print_digit(BCD_DIGIT1(data_1));
//    print_digit(BCD_DIGIT0(data[1]));
    print_digit(BCD_DIGIT0(data_1));
//    print_hex(data[1],2);
//    print_hex(data_1,2);
    print(":");
//    print_digit(BCD_DIGIT1(data[0]));
    print_digit(BCD_DIGIT1(data_0));
//    print_digit(BCD_DIGIT0(data[0]));
    print_digit(BCD_DIGIT0(data_0));
//    print("     ");

    // Display seconds value on LEDs
//    reg_gpio_data = data[0];	// Seconds on LEDs
    reg_gpio_data = data_0;	// Seconds on LEDs
}



void rtc_run()
{
	// Clear STOP bit from register 0x0
	unsigned char send_data = 0x00;
	i2c_send(RTC_I2C_ADDR, 0x00, &send_data, 1);
}



void rtc_stop()
{
	// Apply STOP bit to register 0x0
	unsigned char send_data = 0x10;
	i2c_send(RTC_I2C_ADDR, 0x00, &send_data, 1);
}

void main()
{
    int j;

    // Used by in-memory read routine
    uint32_t func[&i2c_inmem_read_end - &i2c_inmem_read_begin];

    // Use GPIO to signal the state of the system
    reg_gpio_enb =  0xfff0;
    reg_gpio_data = 0x0000;	// Checkpoint 0 on LEDs

    // Preload the in-memory function
    i2c_inmem_load(func);

    // Required I2C for use with the in-memory software fix as written
    i2c_init(128);

    rtc_run();
    
    reg_uart_clkdiv = 6667; // 9600 baud at 8 MHz osc

    // Testbench to be completed:  Set up configuration
    // then read and write a few bytes to confirm signalling.

    while (1) {
        read_rtc(func);
	for (j = 0; j < 70000; j++); // 2 sec
    }	
}
