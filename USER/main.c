#include "rd5767_head.h"
#include <rtthread.h>

int main(void)
{	
	LED_Init();
	BEEP_Init();
	BEEP = 1;
	rt_thread_mdelay(100);
	BEEP = 0;
	rt_thread_mdelay(1000);;
	while(1)
	{
		LED0 = 1;
		rt_thread_mdelay(1000);
		LED0 = 0;
		rt_thread_mdelay(1000);
		LED1 = 1;
		rt_thread_mdelay(1000);
		LED1 = 0;
		rt_thread_mdelay(1000);
	}
}
