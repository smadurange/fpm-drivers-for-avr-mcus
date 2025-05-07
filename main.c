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

#include "r503.h"

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
	cli();
	Soft_UART_init();
	bit_set(DDRB,5);
	sei();
	
	fpm_init();
	if (fpm_get_count() == 0) {
		fpm_enroll();
	}

    while (1) 
    {	
    }

	return 0;
}

