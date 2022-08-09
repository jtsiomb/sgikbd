#ifndef SGI_H_
#define SGI_H_

#include <stdint.h>

enum {
	SGI_LED_NUMLK	= 0x01,
	SGI_LED_CAPSLK	= 0x02,
	SGI_LED_SCRLK	= 0x04,
	SGI_LED_L1		= 0x08,
	SGI_LED_L2		= 0x10,
	SGI_LED_L3		= 0x20,
	SGI_LED_L4		= 0x40
};

extern unsigned char sgi_ledstate;

void sgi_procinp(void);

void sgi_sendkey(int key, int press);
void sgi_sendmouse(uint8_t bnstate, int16_t dx, int16_t dy);

#endif	/* SGI_H_ */
