#include <util/delay.h>

#include "r503.h"

int main(void)
{
	fpm_init();

    for (;;) 
    {	
		fpm_led_on(RED);
		_delay_ms(500);
		fpm_led_off();
		_delay_ms(500);
		fpm_led_on(BLUE);
		_delay_ms(500);
		fpm_led_off();
		_delay_ms(500);
		fpm_led_on(PURPLE);
		_delay_ms(500);
		fpm_led_off();
		_delay_ms(500);
    }

	return 0;
}
