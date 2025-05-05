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

static inline uint8_t aura_on(void)
{
	unsigned int n;
	uint8_t buf[MAXPDLEN];
	
	buf[0] = 0x35;
	buf[1] = 0x01;
	buf[2] = 0x20;
	buf[3] = 0x03;
	buf[4] = 0x00;

	send(0x01, buf, 5);	
	recv(buf, &n);
	return buf[0] == OK;
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
	aura_on();
	return check_pwd();
}

