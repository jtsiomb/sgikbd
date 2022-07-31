#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"
#include "scantbl.h"
#include "ps2kbd.h"
#include "sgikb.h"
#include "defs.h"
#include "timer.h"

enum {
	KF_BRK = 1,
	KF_EXT = 2,
	KF_EXT1 = 4,
};

int main(void)
{
	unsigned int keyflags = 0;
	unsigned char c, keycode;
	int press;

	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	DDRE = 0;
	PORTB = 0xff;
	PORTC = 0xff;
	PORTD = 0xc3;	/* no pull-ups on PS/2 ports, pull-up on RX0 */
	PORTE = 0xff;
	EIMSK = 0;	/* mask external interrupts */
	EICRA = (1 << ISC11) | (1 << ISC01);	/* falling edge interrupts */

	init_timer();

	/* initialize the UART and enable interrupts */
	init_serial(0, 600, 8, PAR_ODD, 1);
	sei();

	EIMSK = (1 << INT0) | (1 << INT1);	/* enable ps/2 clock interrupt */

	ps2setled(0);	/* start with all LEDs off */

	for(;;) {
		sgi_procinp();

		if(ps2pending()) {
			c = ps2read();
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
	}
	return 0;
}
