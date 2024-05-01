#ifndef F_CPU
#ifdef XTAL
#define F_CPU	XTAL
#else
#warning "compiled for 1mhz internal rc osc. serial comms won't work"
#define F_CPU	1000000
#endif
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/power.h>

#define BUF_SZ	16
#define BUF_IDX_MASK	(BUF_SZ - 1)
#define NEXT_IDX(x)	(((x) + 1) & BUF_IDX_MASK)

struct uartdata {
	unsigned char inbuf[BUF_SZ];
	volatile unsigned char in_rd, in_wr;
} ubuf[2];


/* value for USART ctrl reg B: rx/tx enable & rx interrupt */
#define CTLB	((1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0))

void init_serial(int uidx, long baud, int bits, int par, int stop)
{
	static const unsigned int ucszbits[] = {3, 3, 3, 3, 3, 0, 1, 2, 3, 7};
	unsigned int ubrr_val = F_CPU / 16 / baud - 1;
	unsigned int ucsz = ucszbits[bits];
	unsigned int ffmt;

	ffmt = (ucsz & 3) << 1;
	if(par) {
		ffmt |= ((par & 1) | 2) << 4;
	}
	if(stop > 1) {
		ffmt |= 1 << USBS0;	/* 2 stop bits */
	}

	switch(uidx) {
	case 0:
		power_usart0_enable();
		UBRR0H = (unsigned char)(ubrr_val >> 8);
		UBRR0L = (unsigned char)ubrr_val;
		UCSR0B = CTLB | (ucsz & 4);
		UCSR0C = ffmt;
		break;

	case 1:
		power_usart1_enable();
		UBRR1H = (unsigned char)(ubrr_val >> 8);
		UBRR1L = (unsigned char)ubrr_val;
		UCSR1B = CTLB | (ucsz & 4);
		UCSR1C = ffmt;

	default:
		break;
	}

	ubuf[uidx].in_rd = ubuf[uidx].in_wr = 0;
}

int have_input(int uidx)
{
	return ubuf[uidx].in_wr != ubuf[uidx].in_rd;
}

int uart_write(int uidx, unsigned char c)
{
	switch(uidx) {
	case 0:
		while((UCSR0A & (1 << UDRE0)) == 0);
		UDR0 = (unsigned char)c;
		return 0;

	case 1:
		while((UCSR1A & (1 << UDRE1)) == 0);
		UDR1 = (unsigned char)c;
		return 0;
	}

	return -1;
}

int uart_read(int uidx)
{
	int c;
	struct uartdata *u = ubuf + uidx;

	while(u->in_rd == u->in_wr);

	c = u->inbuf[u->in_rd];
	u->in_rd = NEXT_IDX(u->in_rd);
	return c;
}

ISR(USART0_RX_vect)
{
	ubuf[0].inbuf[ubuf[0].in_wr] = UDR0;
	ubuf[0].in_wr = NEXT_IDX(ubuf[0].in_wr);
}

ISR(USART1_RX_vect)
{
	ubuf[1].inbuf[ubuf[1].in_wr] = UDR1;
	ubuf[1].in_wr = NEXT_IDX(ubuf[1].in_wr);
}
