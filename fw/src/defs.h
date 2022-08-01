#ifndef DEFS_H_
#define DEFS_H_

#include <avr/io.h>

/* pin assignments:
 * D2	kb clock (INT0)
 * D3	mouse clock (INT1)
 * D4	kb data
 * D5	mouse data
 */

#define KCLK	2
#define MCLK	3
#define KDATA	4
#define MDATA	5

#define KCLK_BIT	(1 << KCLK)
#define MCLK_BIT	(1 << MCLK)
#define KDATA_BIT	(1 << KDATA)
#define MDATA_BIT	(1 << MDATA)

#endif	/* DEFS_H_ */
