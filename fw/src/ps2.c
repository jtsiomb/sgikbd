#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ps2.h"
#include "defs.h"
#include "timer.h"

#define TIMEOUT	100

#define BUF_SZ	32
#define BUF_IDX_MASK	(BUF_SZ - 1)
#define NEXT_IDX(x) (((x) + 1) & BUF_IDX_MASK)
static volatile unsigned char keybuf[BUF_SZ];
static volatile unsigned char key_rd, key_wr;

struct mpkt {
	unsigned char data[3];
};
static volatile struct mpkt mpktbuf[BUF_SZ];
static volatile unsigned char mpkt_rd, mpkt_wr;

static volatile struct mpkt curpkt;
static volatile unsigned char curpkt_byte;
static volatile int auxstate;

static volatile int send_nbits, wait_ack;
static volatile unsigned char send_val, send_par;

static int ps2write(unsigned char c, unsigned char ckbit, unsigned char dbit);
static void abort_send(unsigned char ckbit, unsigned char dbit);

int ps2kb_write(unsigned char c)
{
	return ps2write(c, KCLK_BIT, KDATA_BIT);
}

static int ps2write(unsigned char c, unsigned char ckbit, unsigned char dbit)
{
	cli();
	send_nbits = 10;
	send_val = c;
	send_par = 0;	/* init to 0, will be calculated while sending */

	/* inhibit transmission, hold at least 100us */
	PORTD &= ~ckbit;
	DDRD |= ckbit;
	EIFR |= (1 << INTF0);	/* clear pending interrupt due to clock transition */
	sei();
	_delay_us(100);

	/* RTS by data low & release clock (this counts as start bit?) */
	PORTD &= ~dbit;
	DDRD |= dbit;
	DDRD &= ~ckbit;

	reset_timer();
	while(send_nbits > 0) {
		if(get_msec() > TIMEOUT) {
			abort_send(ckbit, dbit);
			return -1;
		}
	}
	_delay_us(5);

	/* release data line and wait for ack */
	cli();
	wait_ack = 1;
	sei();
	DDRD &= ~dbit;
	reset_timer();
	while(wait_ack) {
		if(get_msec() > TIMEOUT) {
			abort_send(ckbit, dbit);
			return -1;
		}
	}
	return 0;
}

static void abort_send(unsigned char ckbit, unsigned char dbit)
{
	cli();
	send_nbits = 0;
	wait_ack = 0;
	/* hold clock low for 100us */
	PORTD &= ~ckbit;
	DDRD |= ckbit;
	_delay_us(100);
	DDRD &= ~(ckbit | dbit);
	EIFR |= (1 << INTF0);	/* clear pending interrupt */
	sei();
}

unsigned char ps2kb_read(void)
{
	unsigned char key;

	while(key_rd == key_wr) {
	}

	cli();
	key = keybuf[key_rd];
	key_rd = NEXT_IDX(key_rd);
	sei();

	return key;
}

int ps2kb_pending(void)
{
	return key_rd != key_wr;
}


int ps2kb_wait(unsigned int timeout)
{
	reset_timer();
	while(key_rd == key_wr) {
		if(get_msec() >= timeout) return -1;
	}
	return 0;
}

void ps2kb_clearbuf(void)
{
	key_rd = key_wr;
}

int psaux_write(unsigned char c)
{
	return ps2write(c, MCLK_BIT, MDATA_BIT);
}

void psaux_read(struct mouse_input *inp)
{
	unsigned char pkt[3];
	while(key_rd == key_wr);

	cli();
	pkt[0] = mpktbuf[mpkt_rd].data[0];
	pkt[1] = mpktbuf[mpkt_rd].data[1];
	pkt[2] = mpktbuf[mpkt_rd].data[2];
	mpkt_rd = NEXT_IDX(mpkt_rd);
	sei();

	inp->bnstate = pkt[0] & 7;
	inp->dx = (pkt[0] & 0x10) ? -(short)pkt[1] : (short)pkt[1];
	inp->dy = (pkt[0] & 0x20) ? -(short)pkt[2] : (short)pkt[2];
}

int psaux_pending(void)
{
	return mpkt_rd != mpkt_wr;
}

#define PS2_ACK		0xfa
#define PS2_RESEND	0xfe
#define PS2_ECHO	0xee

int ps2kb_setled(unsigned char state)
{
	unsigned char c;

	ps2kb_write(0xed);
	reset_timer();
	while(!ps2kb_pending()) {
		if(get_msec() >= TIMEOUT) return -1;
	}
	c = ps2kb_read();
	/*printf("ps2setled 1st response: %x\r\n", (unsigned int)c);*/
	if(c != PS2_ACK) return -1;

	ps2kb_write(state);
	reset_timer();
	while(!ps2kb_pending()) {
		if(get_msec() >= TIMEOUT) return -1;
	}
	c = ps2kb_read();
	/*printf("ps2setled 2nd response: %x\r\n", (unsigned int)c);*/
	if(c != PS2_ACK) return -1;

	return 0;
}

ISR(INT0_vect)
{
	static unsigned char value, parity;
	/*static unsigned char valp;*/
	static int nbits;

	if(wait_ack) {
		if(!(PIND & KDATA_BIT)) {
			wait_ack = 0;
		}
		return;
	}

	if(send_nbits > 0) {
		--send_nbits;
		switch(send_nbits) {
		case 1:		/* parity bit */
			if(send_par & 1) {	/* odd number of ones: parity 0 */
				PORTD &= ~KDATA_BIT;
			} else {	/* even number of ones: parity 1 */
				PORTD |= KDATA_BIT;
			}
			break;
		case 0:	/* stop bit: 1 */
			PORTD |= KDATA_BIT;
			break;
		default:
			if(send_val & 1) {
				PORTD |= KDATA_BIT;
				++send_par;
			} else {
				PORTD &= ~KDATA_BIT;
			}
			send_val >>= 1;
		}

	} else {
		if(nbits > 0 && nbits < 9) {
			value >>= 1;
			if(PIND & KDATA_BIT) {
				value |= 0x80;
				parity ^= 1;
			}
		}/* else if(nbits == 9) {
			valp = (PIND >> KDATA) & 1;
		}*/
		if(++nbits >= 11) {
			nbits = 0;

			/* check parity */
			/*if((parity & 1) == (valp & 1)) {}*/
			keybuf[key_wr] = (unsigned char)value;
			key_wr = NEXT_IDX(key_wr);

			value = 0;
			parity = 0;
		}
	}
}

ISR(INT1_vect)
{
	static unsigned char value, parity;
	static int nbits;

	if(wait_ack) {
		if(!(PIND & MDATA_BIT)) {
			wait_ack = 0;
		}
		return;
	}

	if(send_nbits > 0) {
		--send_nbits;
		switch(send_nbits) {
		case 1:		/* parity bit */
			if(send_par & 1) {	/* odd number of ones: parity 0 */
				PORTD &= ~MDATA_BIT;
			} else {	/* even number of ones: parity 1 */
				PORTD |= MDATA_BIT;
			}
			break;
		case 0:	/* stop bit: 1 */
			PORTD |= MDATA_BIT;
			break;
		default:
			if(send_val & 1) {
				PORTD |= MDATA_BIT;
				++send_par;
			} else {
				PORTD &= ~MDATA_BIT;
			}
			send_val >>= 1;
		}

	} else {
		if(nbits > 0 && nbits < 9) {
			value >>= 1;
			if(PIND & MDATA_BIT) {
				value |= 0x80;
				parity ^= 1;
			}
		}/* else if(nbits == 9) {
			valp = (PIND >> MDATA) & 1;
		}*/
		if(++nbits >= 11) {
			nbits = 0;

			/* check parity */
			/*if((parity & 1) == (valp & 1)) {}*/

			switch(auxstate) {
			case 0:		/* not initialized */
				if(value == 0xaa) {
					auxstate++;
				}
				break;

			case 1:		/* expect ID after BAT */
				if(value == 0) {
					auxstate++;
					/* enable data reporting */
					ps2write(0xf4, MCLK_BIT, MDATA_BIT);
				} else {
					auxstate = 0;
				}
				break;

			case 2:		/* expect ACK after enable data reporting command */
				if(value == PS2_ACK) {
					auxstate = 0xff;
				} else {
					auxstate = 0;
				}
				break;

			default:
				curpkt.data[curpkt_byte++] = (unsigned char)value;
				if(curpkt_byte >= 3) {
					mpktbuf[mpkt_wr].data[0] = curpkt.data[0];
					mpktbuf[mpkt_wr].data[1] = curpkt.data[1];
					mpktbuf[mpkt_wr].data[2] = curpkt.data[2];
					mpkt_wr = NEXT_IDX(mpkt_wr);
					curpkt_byte = 0;
				}
			}

			/* XXX debug LEDs */
			PORTC = (auxstate & 3) | 0xfc;

			value = 0;
			parity = 0;
		}
	}
}
