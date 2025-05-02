/*
 * example_usage.c
 *
 */ 

#define bit_clr(a,b) ((a) &=~(1<<(b)))
#define bit_set(a,b) ((a) |= (1<<(b)))
#define bit_tst(a,b) ((a) & (1<<(b)))
#define bit_change(a,b) ((a) ^= (1<<(b)))

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define Soft_UART_TX_PORT PORTD
#define Soft_UART_TX_DDR DDRD
#define Soft_UART_TX_PIN 3
#define Soft_UART_Baud 9600
#include "Soft_UART_Timer1.h"

#include "fpm.h"

static inline void uart_write(const char *s)
{
	for (; *s; s++) {
		Soft_UART_send_byte(*s);
	}
	Soft_UART_send_byte('\r');
	Soft_UART_send_byte('\n');
}

int main(void)
{
	const int SLEN = 25;

	char s[SLEN];
	struct fpm_config cfg;

	cli();
	Soft_UART_init();
	bit_set(DDRB,5);
	sei();

	if (fpm_init())
		uart_write("FPM detected");
	else
		uart_write("FPM not detected");

	if (fpm_get_config(&cfg)) {
		uart_write("FPM config:");
		snprintf(s, SLEN, "\tstatus: 0x%02X", cfg.status);
		uart_write(s);
		snprintf(s, SLEN, "\tsysid: 0x%02X", cfg.sysid);
		uart_write(s);
		snprintf(s, SLEN, "\tcap: %d", cfg.cap);
		uart_write(s);
		snprintf(s, SLEN, "\tsec: %d", cfg.sec_level);
		uart_write(s);
		snprintf(s, SLEN, "\taddr: 0x%02X%02X%02X%02X", cfg.addr[0], 
			cfg.addr[1], cfg.addr[2], cfg.addr[3]);
		uart_write(s);
		snprintf(s, SLEN, "\tpkt size: %d", cfg.pkt_size);
		uart_write(s);
	
		if (cfg.baud == 1)
			uart_write("\tbaud: 9600");
		else if (cfg.baud == 2)
			uart_write("\tbaud: 19200");
		else if (cfg.baud == 3)
			uart_write("\tbaud: 28800");
		else if (cfg.baud == 4)
			uart_write("\tbaud: 38400");
		else if (cfg.baud == 5)
			uart_write("\tbaud: 48000");
		else if (cfg.baud == 6)
			uart_write("\tbaud: 57600");
		else if (cfg.baud == 7)
			uart_write("\tbaud: 67200");
		else if (cfg.baud == 8)
			uart_write("\tbaud: 76800");
		else if (cfg.baud == 9)
			uart_write("\tbaud: 86400");
		else if (cfg.baud == 10)
			uart_write("\tbaud: 96000");
		else if (cfg.baud == 11)
			uart_write("\tbaud: 105600");
		else if (cfg.baud == 12)
			uart_write("\tbaud: 115200");
	}
	
    while (1) 
    {	
		_delay_ms(1000);
    }

	return 0;
}
