/*
 * Title:			AGON timer interface
 * Author:			Jeroen Venema
 * Created:			06/11/2022
 * Last Updated:	22/01/2023
 * 
 * Modinfo:
 * 06/11/2022:		Initial version
 * 22/01/2023:      Freerunning timer0 code added, needs interrupt handler code
 */

#include <defines.h>

#ifndef AGONTIMER_H
#define AGONTIMER_H

void delayms(int ms);

#endif AGONTIMER_H
