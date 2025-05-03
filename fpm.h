#ifndef FPM_H
#define FPM_H

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

uint8_t fpm_init(void);

uint8_t fpm_getcfg(struct fpm_cfg *cfg);

uint8_t fpm_setpwd(uint32_t pwd);

uint16_t fpm_getcount(void);

uint8_t fpm_enroll(void);

uint8_t fpm_match(void);

#endif  /* FPM_H */
