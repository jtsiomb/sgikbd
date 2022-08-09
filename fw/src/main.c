#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"
#include "scantbl.h"
#include "ps2.h"
#include "sgi.h"
#include "defs.h"
#include "timer.h"

enum {
	KF_BRK = 1,
	KF_EXT = 2,
	KF_EXT1 = 4,
};

void proc_atkbd(void);
void proc_psaux(void);

int main(void)
{
	unsigned char prevled;

	DDRB = 0;
	DDRC = 3;	/* debug out */
	DDRD = 0;
	DDRE = 0;
	PORTB = 0xff;
	PORTC = 0xfc;
	PORTD = 0xff;	/* pull-ups on PS/2 ports, pull-up on RX0 */
	PORTE = 0xff;
	EIMSK = 0;	/* mask external interrupts */
	EICRA = (1 << ISC11) | (1 << ISC01);	/* falling edge interrupts */

	init_timer();

	/* initialize the UART and enable interrupts */
	init_serial(0, 600, 8, PAR_ODD, 1);		/* SGI keyboard */
	init_serial(1, 4800, 8, PAR_NONE, 1);	/* SGI mouse */
	sei();

	EIMSK = (1 << INT0) | (1 << INT1);	/* enable ps/2 clock interrupts */

	ps2kb_setled(0);	/* start with all LEDs off */

	for(;;) {
		prevled = sgi_ledstate;

		sgi_procinp();

		if(sgi_ledstate ^ prevled) {
			ps2kb_setled(((sgi_ledstate & 3) << 1) | ((sgi_ledstate & 4) >> 2));
		}

		proc_atkbd();
		proc_psaux();
	}
	return 0;
}

void proc_atkbd(void)
{
	static unsigned int keyflags;
	unsigned char c, keycode;
	int press;

	if(!ps2kb_pending()) return;

	c = ps2kb_read();
	switch(c) {
	case 0xe0:	/* extended */
		keyflags |= KF_EXT;
		break;

	case 0xe1:	/* extended */
		keyflags |= KF_EXT1;
		break;

	case 0xf0:	/* break code */
		keyflags |= KF_BRK;
		break;

	default:
		keycode = 0xff;
		if(keyflags & KF_EXT) {
			if(c < KEYMAP_EXT_SIZE) {
				keycode = keymap_ext[(int)c];
			}
		} else if(keyflags & KF_EXT1) {
		} else {
			if(c < KEYMAP_NORMAL_SIZE) {
				keycode = keymap_normal[(int)c];
			}
		}

		if(keycode != 0xff) {
			press = ~keyflags & KF_BRK;
			sgi_sendkey(keycode, press);
		}
		keyflags = 0;
	}
}

void proc_psaux(void)
{
	struct mouse_input inp;

	if(!psaux_pending()) return;

	psaux_read(&inp);

	sgi_sendmouse(inp.bnstate, inp.dx, inp.dy);
}
