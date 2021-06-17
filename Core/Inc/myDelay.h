/*
 * myDelay.h
 *
 *  Created on: 14 jun. 2021
 *      Author: aleja
 */

#ifndef INC_MYDELAY_H_
#define INC_MYDELAY_H_

#include "stdint.h"

typedef struct{
   uint32_t startTime;
   uint16_t duration;
   uint8_t running;
} delay_t;

void delayConfig( delay_t * delay, uint16_t duration );
uint8_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, uint16_t duration );


#endif /* INC_MYDELAY_H_ */
