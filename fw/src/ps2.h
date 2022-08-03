#ifndef PS2_H_
#define PS2_H_

#include <stdint.h>

enum {
	PS2LED_SCRLK = 1,
	PS2LED_NUMLK = 2,
	PS2LED_CAPSLK = 4
};

struct mouse_input {
	uint8_t bnstate;
	int16_t dx, dy;
};

int ps2kb_write(unsigned char c);
unsigned char ps2kb_read(void);
int ps2kb_pending(void);
int ps2kb_wait(unsigned int timeout);
void ps2kb_clearbuf(void);

int psaux_write(unsigned char c);
void psaux_read(struct mouse_input *inp);
int psaux_pending(void);

int ps2kb_setled(unsigned char state);

#endif	/* PS2_H_ */
