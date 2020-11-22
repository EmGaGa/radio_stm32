#ifndef __LED_H
#define __LED_H

#include "REG51.h"

sbit LED=P2^1;

void delay_ms(unsigned int ms);
void LED_Toggle(void);

#endif