/*
 * example_usage.c
 *
 */ 

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "fpm.h"

#define TOUCH_PIN        PB0
#define TOUCH_DDR        DDRB
#define TOUCH_PORT       PORTB
#define TOUCH_PCIE       PCIE0
#define TOUCH_PCINT      PCINT0
#define TOUCH_PCMSK      PCMSK0
#define TOUCH_PCINTVEC   PCINT0_vect

static inline void wdt_off(void)
{
	cli();
	wdt_reset();
	MCUSR &= ~(1 << WDRF);
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = 0x00;
}

int main(void)
{
	char s[30];
	uint8_t ismatch;
	uint16_t template_count;

	wdt_off();

	//cli();
	//Soft_UART_init();
	//bit_set(DDRB,5);

	TOUCH_DDR &= ~(1 << TOUCH_PIN);
	PCICR |= (1 << TOUCH_PCIE);
	TOUCH_PCMSK |= (1 << TOUCH_PCINT);

	DDRB |= (1 << PB5);
	PORTB &= ~(1 << PB5);

	sei();

	if (fpm_init()) {
		//print_config();
		//template_count = fpm_getcount();
		//snprintf(s, 30, "Template count: %d", template_count); 
		//uart_write(s);

		//if (template_count == 0) {
		//	uart_write("Enroll fingerprint");
		//	if (fpm_enroll()) {
		//		uart_write("Enrolled fingerprint"); 
		//	} else {
		//		uart_write("Enrollment error");
		//	}

		//	template_count = fpm_getcount();
		//	snprintf(s, 30, "New template count: %d", template_count); 
		//	uart_write(s);
		//}
	}

    while (1) 
    {	
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();	
		
		if (fpm_match()) {
			PORTB |= (1 << PB5);	
			_delay_ms(500);
			PORTB &= ~(1 << PB5);	
		}
    }

	return 0;
}

ISR(TOUCH_PCINTVEC)
{
}
