#include "serial.h"

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
