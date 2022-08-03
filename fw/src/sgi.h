#ifndef SGI_H_
#define SGI_H_

#include <stdint.h>

void sgi_procinp(void);

void sgi_sendkey(int key, int press);
void sgi_sendmouse(uint8_t bnstate, int16_t dx, int16_t dy);

#endif	/* SGI_H_ */
