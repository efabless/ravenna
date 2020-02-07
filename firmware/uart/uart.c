#include "../ravenna_defs.h"

#define BCD_DIGIT0(x) (x & (uint32_t)0x000F)
#define BCD_DIGIT1(x) ((x >> 4) & (uint32_t)0x000F)

// --------------------------------------------------------
// Firmware routines
// --------------------------------------------------------

// Copy the flash worker function to SRAM so that the SPI can be
// managed without having to read program instructions from it.

void flashio(uint32_t *data, int len, uint8_t wrencmd)
{
	uint32_t func[&flashio_worker_end - &flashio_worker_begin];

	uint32_t *src_ptr = &flashio_worker_begin;
	uint32_t *dst_ptr = func;

	while (src_ptr != &flashio_worker_end)
		*(dst_ptr++) = *(src_ptr++);

	((void(*)(uint32_t*, uint32_t, uint32_t))func)(data, len, wrencmd);
}

//--------------------------------------------------------------
// NOTE: Volatile write *only* works with command 01, making the
// above routing non-functional.  Must write all four registers
// status, config1, config2, and config3 at once.
//--------------------------------------------------------------
// (NOTE: Forces quad/ddr modes off, since function runs in single data pin mode)
// (NOTE: Also sets quad mode flag, so run this before entering quad mode)
//--------------------------------------------------------------

void set_flash_latency(uint8_t value)
{
	reg_spictrl = (reg_spictrl & ~0x007f0000) | ((value & 15) << 16);

	uint32_t buffer_wr[2] = {0x01000260, ((0x70 | value) << 24)};
	flashio(buffer_wr, 5, 0x50);
}

//--------------------------------------------------------------

void putchar(char c)
{
	if (c == '\n')
		putchar('\r');
	reg_uart_data = c;
}

void print(const char *p)
{
	while (*p)
		putchar(*(p++));
}

void print_ln(const char *p)
{
	for (int i=0; i<20; i++)
	{
	    if (*p)
		    putchar(*(p++));
        else
		    putchar(' ');
    }
}

void clear()
{
    reg_uart_data = 0x7c;
    reg_uart_data = 0x2d;
}

void home()
{
    reg_uart_data = 254;
    reg_uart_data = 0x02;
}

void print_hex(uint32_t v, int digits)
{
	for (int i = digits - 1; i >= 0; i--) {
		char c = "0123456789abcdef"[(v >> (4*i)) & 15];
		putchar(c);
	}
}

void print_dec(uint32_t v)
{
	if (v >= 2000) {
		print("OVER");
		return;
	}
	else if (v >= 1000) { putchar('1'); v -= 1000; }
	else putchar(' ');

	if 	(v >= 900) { putchar('9'); v -= 900; }
	else if	(v >= 800) { putchar('8'); v -= 800; }
	else if	(v >= 700) { putchar('7'); v -= 700; }
	else if	(v >= 600) { putchar('6'); v -= 600; }
	else if	(v >= 500) { putchar('5'); v -= 500; }
	else if	(v >= 400) { putchar('4'); v -= 400; }
	else if	(v >= 300) { putchar('3'); v -= 300; }
	else if	(v >= 200) { putchar('2'); v -= 200; }
	else if	(v >= 100) { putchar('1'); v -= 100; }
	else putchar('0');
		
	if 	(v >= 90) { putchar('9'); v -= 90; }
	else if	(v >= 80) { putchar('8'); v -= 80; }
	else if	(v >= 70) { putchar('7'); v -= 70; }
	else if	(v >= 60) { putchar('6'); v -= 60; }
	else if	(v >= 50) { putchar('5'); v -= 50; }
	else if	(v >= 40) { putchar('4'); v -= 40; }
	else if	(v >= 30) { putchar('3'); v -= 30; }
	else if	(v >= 20) { putchar('2'); v -= 20; }
	else if	(v >= 10) { putchar('1'); v -= 10; }
	else putchar('0');

	if 	(v >= 9) { putchar('9'); v -= 9; }
	else if	(v >= 8) { putchar('8'); v -= 8; }
	else if	(v >= 7) { putchar('7'); v -= 7; }
	else if	(v >= 6) { putchar('6'); v -= 6; }
	else if	(v >= 5) { putchar('5'); v -= 5; }
	else if	(v >= 4) { putchar('4'); v -= 4; }
	else if	(v >= 3) { putchar('3'); v -= 3; }
	else if	(v >= 2) { putchar('2'); v -= 2; }
	else if	(v >= 1) { putchar('1'); v -= 1; }
	else putchar('0');
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

char getchar_prompt(char *prompt)
{
	int32_t c = -1;

	uint32_t cycles_begin, cycles_now, cycles;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));

//	reg_leds = ~0;

	if (prompt)
		print(prompt);

	while (c == -1) {
		__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
		cycles = cycles_now - cycles_begin;
		if (cycles > 12000000) {
			if (prompt)
				print(prompt);
			cycles_begin = cycles_now;
//			reg_leds = ~reg_leds;
		}
		c = reg_uart_data;
	}

//	reg_leds = 0;
	return c;
}

char getchar()
{
	return getchar_prompt(0);
}

// ----------------------------------------------------------------------

void cmd_read_flash_regs_print(uint32_t addr, const char *name)
{
    uint32_t buffer[2] = {0x65000000 | addr, 0x0}; //
    flashio(buffer, 6, 0);

    print("0x");
    print_hex(addr, 6);
    print(" ");
    print(name);
    print(" 0x");
    print_hex(buffer[1], 2);
    print("  ");
}

void cmd_echo()
{
	print("Return to menu by sending '!'\n\n");
	char c;
	while ((c = getchar()) != '!') {
		if (c == '\r')
		    putchar('\n');
        else
		    putchar(c);
    }
}

// ----------------------------------------------------------------------

void main()
{
	uint32_t i;

	uint32_t count;

	set_flash_latency(8);


	// NOTE: Crystal on testboard running at 12.5MHz
	// Internal clock is 8x crystal, or 100MHz
	// Divided by clkdiv is 9.6 kHz
	// So at this crystal rate, use clkdiv = 10417 for 9600 baud.

	// Set UART clock to 9600 baud
    //	reg_uart_clkdiv = 10417;
//	reg_uart_clkdiv = 8333;
	reg_uart_clkdiv = 6667;

	reg_gpio_enb
	 = 0x0000;
	reg_gpio_data = 0x000f;


	// Need boot-up time for the display;  give it 4 seconds
    //	for (j = 0; j < 700000 * m; j++);

	// This should appear on the LCD display 4x20 characters.
    print_ln("Starting...\n");

    print("Press ENTER to continue..\n");
    while (getchar() != '\r') {}

    //    cmd_echo();

    print("\n\n");

    for(count=0;;count++) {
		reg_gpio_data = (count >> 16);
//        for (j = 0; j < 17000; j++); // 2 sec
        if (count && 0xff) == 0 putchar('.');
    }
}

