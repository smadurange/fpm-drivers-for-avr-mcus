#ifndef FPM_R503_H
#define FPM_R503_H

#include <stdint.h>

typedef enum {
	RED = 0x01,
	BLUE = 0x02,
	PURPLE = 0x03
} FPM_LED_COLOR;

uint8_t fpm_init(void);

void fpm_led_on(FPM_LED_COLOR color);

void fpm_led_off(void);

#endif /* FPM_R50_H */
