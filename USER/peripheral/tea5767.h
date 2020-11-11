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


#endif
