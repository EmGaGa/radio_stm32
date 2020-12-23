#ifndef __TEA5767_H
#define __TEA5767_H 	
#include "iic.h"

/* api func */
void tea5767_init(void);
void tea5767_write(void);
void tea5767_read(void);
void get_tea5767_pll(void);
void get_tea5767_frequency(void);
void tea5767_search(int mode);
void tea5767_auto_search(int mode);

/* info */
typedef struct
{
	unsigned int cur_pll;
	unsigned long cur_freq;			/* µ±Ç°Æµ¶Î */
}radio_sys_info_t;

void get_radio_sys_pll(radio_sys_info_t *radio_info);
void get_radio_sys_frequency(radio_sys_info_t *radio_info);

#endif
