#include "serial.h"
#include "sgi.h"

#define KBCTL_SEL(c)	((c) & 1)

#define KBCTL_SEL0	0
#define KBCTL_SEL1	1

enum {
	KBCTL0_SBEEP	= 0x02,
	KBCTL0_LBEEP	= 0x04,
	KBCTL0_NOCLICK	= 0x08,
	KBCTL0_GETCFG	= 0x10,
	KBCTL0_DS1		= 0x20,
	KBCTL0_DS2		= 0x40,
	KBCTL0_AUTOREP	= 0x80
};
enum {
	KBCTL1_INVDS12	= 0x02,
	KBCTL1_DS3		= 0x04,
	KBCTL1_DS4		= 0x08,
	KBCTL1_DS5		= 0x10,
	KBCTL1_DS6		= 0x20,
	KBCTL1_DS7		= 0x40
};



void sgi_procinp(void)
{
	unsigned char c;

	while(have_input(0)) {
		c = uart_read(0);

		/* handle control bytes */
		if(KBCTL_SEL(c) == KBCTL_SEL0) {
			if(c & KBCTL0_GETCFG ) {
				uart_write(0, 110);
				uart_write(0, 0);	/* DIP switches */
			}
		}
	}
}

void sgi_sendkey(int key, int press)
{
	if(!press) key |= 0x80;

	uart_write(0, key);
}

void sgi_sendmouse(uint8_t bn, int16_t dx, int16_t dy)
{
	unsigned char pkt[5];
	int16_t x1, y1, x2, y2;

	if(dx > 127) {
		x1 = 127;
	} else if(dx < -128) {
		x1 = -128;
	} else {
		x1 = dx;
	}
	x2 = dx - x1;

	if(dy > 127) {
		y1 = 127;
	} else if(dy < -128) {
		y1 = -128;
	} else {
		y1 = dy;
	}
	y2 = dy - y1;

	pkt[0] = 0x80 | ((bn & 1) << 2) | ((bn & 2) << 1) | ((bn & 4) >> 2);
	pkt[1] = *(unsigned char*)&x1;
	pkt[2] = *(unsigned char*)&y1;
	pkt[3] = *(unsigned char*)&x2;
	pkt[4] = *(unsigned char*)&y2;

	uart_write(1, pkt[0]);
	uart_write(1, pkt[1]);
	uart_write(1, pkt[2]);
	uart_write(1, pkt[3]);
	uart_write(1, pkt[4]);
}
