#ifndef FPM_R503_H
#define FPM_R503_H

#include <stdint.h>

struct fpm_cfg {
	uint16_t status;
	uint16_t sysid;
	uint16_t cap;
	uint16_t sec_level;
	uint8_t addr[4];
	uint16_t pkt_size;
	uint16_t baud;
};

typedef enum {
	RED = 0x01,
	BLUE = 0x02,
	PURPLE = 0x03
} COLOR;

uint8_t fpm_init(void);

uint8_t fpm_get_cfg(struct fpm_cfg *cfg);

void fpm_led_on(COLOR color);

void fpm_led_off(void);

uint8_t fpm_clear_db(void);

uint16_t fpm_get_count(void);

uint8_t fpm_enroll(void);

#endif /* FPM_R50_H */
