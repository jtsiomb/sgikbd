#ifndef SERIAL_H_
#define SERIAL_H_

enum { PAR_NONE, PAR_ODD, PAR_EVEN };

void init_serial(int uidx, long baud, int bits, int par, int stop);
int have_input(int uidx);

int uart_write(int uidx, unsigned char c);
int uart_read(int uidx);


#endif	/* SERIAL_H_ */
