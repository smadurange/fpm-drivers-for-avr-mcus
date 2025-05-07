#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include "r503.h"

#define MAXPDLEN              64
#define RST_DELAY_MS         500

#define HEADER_HO           0xEF
#define HEADER_LO           0x01
#define ADDR          0xFFFFFFFF

#define OK                  0x00

static inline uint8_t read(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}

static inline void write(uint8_t c)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = c;
}

static inline void send(uint8_t pid, uint8_t *data, uint8_t n)
{
	int i;
	uint16_t pktlen, sum;

	write(HEADER_HO);
	write(HEADER_LO);

	write((uint8_t)(ADDR >> 24));
	write((uint8_t)(ADDR >> 16));
	write((uint8_t)(ADDR >> 8));
	write((uint8_t)(ADDR & 0xFF));

	write(pid);

	pktlen = n + 2;
	write((uint8_t)(pktlen >> 8));
	write((uint8_t)pktlen);

	sum = (pktlen >> 8) + (pktlen & 0xFF) + pid;
	for (i = 0; i < n; i++) {
		write(data[i]);
		sum += data[i];
	}

	write((uint8_t)(sum >> 8));
	write((uint8_t)sum);
}

static inline void recv(uint8_t buf[MAXPDLEN], uint16_t *n)
{
	int i;
	uint16_t len;
	uint8_t byte;

	i = 0, len = 0;

	for (;;) {
		byte = read();
		switch (i) {
		case 0:
			if (byte != HEADER_HO)
				continue;
			break;
		case 1:
			if (byte != HEADER_LO)
				goto bad_pkt;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			// toss the address
			break;
		case 6:
			// toss the packet id
			break;
		case 7:
			len = (uint16_t)byte << 8;
			break;
		case 8:
			len |= byte;
			break;
		default:
			if ((i - 9) < MAXPDLEN) {
				buf[i - 9] = byte;
				if ((i - 8) == len) {
					*n = len;
					return;
				}
			} else {
				goto bad_pkt;
			}
			break;
		}
		i++;
	}

bad_pkt:
	*n = 0;
	return;
}

static inline void led_ctrl(uint8_t mode, COLOR color)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];
	
	buf[0] = 0x35;
	buf[1] = mode;
	buf[2] = 0x60;
	buf[3] = color;
	buf[4] = 0x00;

	send(0x01, buf, 5);	
	recv(buf, &n);
}

static inline uint8_t check_pwd(void)
{
	unsigned int n;
	uint8_t buf[MAXPDLEN];

	buf[0] = 0x13;
	buf[1] = (uint8_t)((uint32_t)FPM_PWD >> 24);
	buf[2] = (uint8_t)((uint32_t)FPM_PWD >> 16);
	buf[3] = (uint8_t)((uint32_t)FPM_PWD >> 8);
	buf[4] = (uint8_t)((uint32_t)FPM_PWD & 0xFF);

	send(0x01, buf, 5);
	recv(buf, &n);
	return buf[0] == OK;
}

static inline uint8_t scan(void)
{
	uint16_t n, retries;
	uint8_t buf[MAXPDLEN];
	
	retries = 0;
	led_ctrl(0x01, PURPLE);

	do {
		buf[0] = 0x28;
		send(0x01, buf, 1);
		recv(buf, &n);
		if (buf[0] != OK) {
			retries++;
			_delay_ms(100);
		}
	} while(buf[0] != OK && retries < 100);

	led_ctrl(0x06, PURPLE);
	return buf[0] == OK;
}

static inline uint8_t img2tz(uint8_t bufid)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];
	
	buf[0] = 0x02;	
	buf[1] = bufid;
	send(0x01, buf, 2);
	recv(buf, &n);
	return buf[0] == OK;
}

void fpm_led_on(COLOR color)
{
	led_ctrl(0x03, color);
}

void fpm_led_off(void)
{
	led_ctrl(0x04, 0x00);
}

uint8_t fpm_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	_delay_ms(RST_DELAY_MS);
	return check_pwd();
}

uint8_t fpm_get_cfg(struct fpm_cfg *cfg)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];

	buf[0] = 0x0F;
	send(0x01, buf, 1);
	recv(buf, &n);

	if (buf[0] == OK && n >= 17) {
		cfg->status = ((uint16_t)buf[1] << 8) | buf[2];
		cfg->sysid = ((uint16_t)buf[3] << 8) | buf[4];
		cfg->cap = ((uint16_t)buf[5] << 8) | buf[6];
		cfg->sec_level = ((uint16_t)buf[7] << 8) | buf[8];
		cfg->addr[0] = buf[9];
		cfg->addr[1] = buf[10];
		cfg->addr[2] = buf[11];
		cfg->addr[3] = buf[12];
		cfg->pkt_size = ((uint16_t)buf[13] << 8) | buf[14];
		cfg->pkt_size = 1 << (cfg->pkt_size + 5); 
		cfg->baud = (((uint16_t)buf[15] << 8) | buf[16]);

		return 1;
	}
	return 0;
}

uint8_t fpm_set_pwd(uint32_t pwd)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];

	buf[0] = 0x12;
	buf[1] = (uint8_t)(pwd >> 24);
	buf[2] = (uint8_t)(pwd >> 16);
	buf[3] = (uint8_t)(pwd >> 8);
	buf[4] = (uint8_t)(pwd & 0xFF);

	send(0x01, buf, 5);
	recv(buf, &n);
	return buf[0] == OK;
}

uint16_t fpm_get_count(void)
{
	uint16_t n, count;
	uint8_t buf[MAXPDLEN];

	buf[0] = 0x1D;
	send(0x01, buf, 1);
	recv(buf, &n);

	count = 0;
	if (buf[0] == OK && n >= 2) {
		count = buf[1];
		count <<= 8;
		count |= buf[2];
	}
	return count;
}

uint8_t fpm_enroll(uint16_t id)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];

	if (!scan())
		return 0;

	if (!img2tz(1))
		return 0;
	
	_delay_ms(2000);

	if (!scan())
		return 0;

	if (!img2tz(2))
		return 0;

	buf[0] = 0x05;
	send(0x01, buf, 1);
	recv(buf, &n);
	if (buf[0] != OK)
		return 0;

	buf[0] = 0x06;
	buf[1] = 1;
	buf[2] = (uint8_t)(id >> 8);
	buf[3] = (uint8_t)(id & 0xFF);
	send(0x01, buf, 4);
	recv(buf, &n);

	return buf[0] == OK;
}

uint8_t fpm_match(void)
{
	struct fpm_cfg cfg;
	uint16_t n;
	uint8_t buf[MAXPDLEN];

	if (!fpm_get_cfg(&cfg))
		return 0;
	
	if (!scan())
		return 0;

	if (!img2tz(1))
		return 0;

	buf[0] = 0x04;
	buf[1] = 1;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = (uint8_t)(cfg.cap >> 8);
	buf[5] = (uint8_t)(cfg.cap & 0xFF);
	
	send(0x01, buf, 6);
	recv(buf, &n);
	return buf[0] == OK;
}

uint8_t fpm_clear_db(void)
{
	uint16_t n;
	uint8_t buf[MAXPDLEN];

	buf[0] = 0x0D;
	send(0x01, buf, 1);
	recv(buf, &n);
	return buf[0] == OK;
}
