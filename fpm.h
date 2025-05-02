#ifndef FPM_H
#define FPM_H

#include <stdint.h>

struct fpm_config {
	uint16_t status;
	uint16_t sysid;
	uint16_t cap;
	uint16_t sec_level;
	uint8_t addr[4];
	uint16_t pkt_size;
	uint16_t baud;
};

uint8_t fpm_init(void);

uint8_t fpm_get_config(struct fpm_config *cfg);

#endif  /* FPM_H */
