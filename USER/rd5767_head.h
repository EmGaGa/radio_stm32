#ifndef __TEA5767_H
#define __TEA5767_H 	
#include "stm32f10x.h"

#include <stdlib.h>
#include <stdio.h>

/* sys */
#include "rtthread.h"
/* peripheral */
#include "tea5767.h"
#include "beep.h"
#include "led.h"
#include "oled.h"
#include "bsp_tim.h"
#include "key.h"
#include "iic.h"

/* info */
typedef struct
{
	unsigned int cur_pll;
	unsigned long cur_freq;			/* µ±Ç°Æµ¶Î */
}radio_sys_info_t;

#endif

