#include "rd5767_head.h"
#include <rtthread.h>

int main(void)
{	
	BEEP_Init();
	tea5767_init();
	uart_init(115200);
	BEEP = 1;
	rt_thread_mdelay(100);
	BEEP = 0;
	rt_thread_mdelay(100);;
	tea5767_write();
	rt_thread_mdelay(100);
	tea5767_read();
	while(1)
	{
		BEEP = 1;
		rt_thread_mdelay(1000);
		BEEP = 0;
		rt_thread_mdelay(1000);
	}
}
