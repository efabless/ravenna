#include "../ravenna_defs.h"

#define BCD_DIGIT0(x) (x & (uint32_t)0x000F)
#define BCD_DIGIT1(x) ((x >> 4) & (uint32_t)0x000F)

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

void print_value(uint32_t v)
{
    uint32_t r, d;

    d = v / 10000;	
    r = v % 10000;	

    print_digit(d);

    d = r / 1000;	
    r = r % 1000;	

    print_digit(d);

    d = r / 100;
    r = r % 100;

    print_digit(d);

    d = r / 10;
    r = r % 10;

    print_digit(d);
    print_digit(r);
}


// --------------------------------------------------------------
// NVRAM configuration register:
//
//	upper 26 bits zero
//	bit 5:	rdy	(read-only) NVRAM idle (not programming)
//	bit 4:	ena	enable NVRAM
//	bit 3:	hs	store (program)
//	bit 2:	hr	recall
//	bit 1:	mem_sel	select bank (0 or 1, normally 0)
//	bit 0:	mem_all select all banks (normally leave 0)
// --------------------------------------------------------------

void main()
{
	// NVRAM testbench
	// 1. Test basic SRAM mode reads/writes

	int i, v, l, k;

	// Set GPIO10 to read NVRAM clock
	reg_gpio_enb = 0xf0000;
	reg_gpio_data = 0x0000;
	reg_pll_out_dest = 3;

	// Set UART for 9600 baud at 8MHz osc.
	reg_uart_clkdiv = 6667;

	// Set clkdiv to 15 to make NVRAM clock 4MHz.
	reg_nvram_clkdiv = 15;

	for (i = 0; i < 32; i++) {
	    *(&reg_nvram_datatop + i) = i * 2 - 1;
	}

	for (i = 0; i < 32; i++) {
	    v = *(&reg_nvram_datatop + i);
	    print_value(i * 2 - 1);
	    print(" : ");
	    print_value(v);
	    print("\n");
	}
	print("(not) programmimg.\n");

	// 2. NV Program bank 1 (128 x 32 bits)
	// This was done once and has been commented out for
	// additional runs.
	// reg_nvramctrl = 0x08;
	// reg_nvramctrl = 0x00;
	// while ((reg_nvramctrl & 0x10) == 0);

	print("done.\n");
	for (k = 0; k < 70000; k++);	// Pause

	// Continuous loop through overwrite & refresh

	l = 3;
	while (1) {

	    print("write new.\n");

	    // 3. Overwrite data with volatile contents
	    for (i = 0; i < 32; i++) {
	        *(&reg_nvram_datatop + i) = i * 7 + l;
	    }

	    // 4. Read back and print temporary data
	    for (i = 0; i < 32; i++) {
	        v = *(&reg_nvram_datatop + i);
	        print_value(i * 7 + l);
	        print(" : ");
	        print_value(v);
	        print("\n");
	    }
  	    for (k = 0; k < 70000; k++);	// Pause
	    print("refresh.\n");

	    // 5. NV Refresh bank 1
	    reg_nvramctrl = 0x04;
	    reg_nvramctrl = 0x00;
	    while ((reg_nvramctrl & 0x10) == 0);

	    print("done.\n");
  	    for (k = 0; k < 70000; k++);	// Pause

	    print("read nvdata.\n");

	    // 6. Read back NV data
	    for (i = 0; i < 32; i++) {
	        v = *(&reg_nvram_datatop + i);
	        print_value(i * 2 - 1);
	        print(" : ");
	        print_value(v);
	        print("\n");
	    }

  	    for (k = 0; k < 70000; k++);	// Pause
	    print("done.\n");
	    l++;
	}
}

