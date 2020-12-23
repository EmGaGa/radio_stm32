#include "rd5767_head.h"
#include "bmp.h"

static radio_sys_info_t radio_info;
static int redraw = 0;

/* define */
#define RUN_LED_THREAD_PRIORITY 	7
#define RUN_LED_THREAD_STACK_SIZE 	256
#define RUN_LED_THREAD_TIMESLICE 	10

#define SCAN_KEY_THREAD_PRIORITY 	4
#define SCAN_KEY_THREAD_STACK_SIZE 	256
#define SCAN_KEY_THREAD_TIMESLICE 	5

#define OLED_DISPLAY_THREAD_PRIORITY 	3
#define OLED_DISPLAY_THREAD_STACK_SIZE 	256
#define OLED_DISPLAY_THREAD_TIMESLICE 	10

/* static val */
static rt_thread_t led_tid = RT_NULL;
static rt_thread_t key_tid = RT_NULL;
static rt_thread_t oled_tid = RT_NULL;

/* local func */
static void _radio_info_init(void);
static void _power_on_init(void);
static void _beep_power_on_prompt(void);
static void _create_thread(void);
static void _run_led_thread_entry(void *parameter);
static void _scan_key_thread_entry(void *parameter);
static void _oled_display_thread_entry(void *parameter);

static void _radio_info_init(void)
{
	memset(&radio_info, 0, sizeof(radio_sys_info_t));
	get_tea5767_pll();
	get_tea5767_frequency();
	tea5767_search(1);
	tea5767_search(0);
	get_radio_sys_pll(&radio_info);
	get_radio_sys_frequency(&radio_info);
}


static void _power_on_init(void)
{
	/* peripheral init */
	BEEP_Init();
	delay_init();		//延时函数初始化  

	LED_Init();
	KEY_Init();  		//初始化与按键连接的硬件接口
	OLED_Init();		//初始化OLED  
	OLED_Clear();

	tea5767_init();
	/* data init */
	_radio_info_init();
}

static void _beep_power_on_prompt(void)
{
	BEEP = 1;
	rt_thread_mdelay(1000);
	BEEP = 0;
	rt_thread_mdelay(1000);	
}


static void _create_thread(void)
{
	/* run led */
	led_tid = rt_thread_create("run_led_thread",
								_run_led_thread_entry, RT_NULL,
								RUN_LED_THREAD_STACK_SIZE,
								RUN_LED_THREAD_PRIORITY, RUN_LED_THREAD_TIMESLICE);
	/* 启动线程 */
	if (led_tid != RT_NULL)
		rt_thread_startup(led_tid);

	/* scan key */
	key_tid = rt_thread_create("scan_key_thread",
								_scan_key_thread_entry, RT_NULL,
								SCAN_KEY_THREAD_STACK_SIZE,
								SCAN_KEY_THREAD_PRIORITY, SCAN_KEY_THREAD_TIMESLICE);
	/* 启动线程 */
	if (key_tid != RT_NULL)
		rt_thread_startup(key_tid);

	/* oled display */
	oled_tid = rt_thread_create("oled_display_thread",
								_oled_display_thread_entry, RT_NULL,
								OLED_DISPLAY_THREAD_STACK_SIZE,
								OLED_DISPLAY_THREAD_PRIORITY, OLED_DISPLAY_THREAD_TIMESLICE);
	/* 启动线程 */
	if (oled_tid != RT_NULL)
		rt_thread_startup(oled_tid);	
}

/* 线 程 1 的 入 口 函 数 */
static void _run_led_thread_entry(void *parameter)
{
	rt_kprintf("%s\n", __func__);

	/* main loop... */
	while (1)
	{
		LED0 = 1;
		rt_thread_mdelay(500);
		LED0 = 0;
		rt_thread_mdelay(500);
		LED1 = 1;
		rt_thread_mdelay(500);
		LED1 = 0;
		rt_thread_mdelay(500);
	}
}

static void _scan_key_thread_entry(void *parameter)
{
	u8 key=0;
	radio_sys_info_t radio_info_old;
	radio_sys_info_t radio_info_new;
	
	rt_kprintf("%s\n", __func__);	

	/* main loop... */
	while(1)
	{
		key=KEY_Scan(0);	//得到键值
		if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:
					BEEP=!BEEP;
					break;
				
				case KEY1_PRES:
					tea5767_search(1);
					break;
				
				case KEY0_PRES:
					get_radio_sys_frequency(&radio_info_old);
					tea5767_search(0);
					get_radio_sys_frequency(&radio_info_new);
					if((radio_info_old.cur_freq/1000 >= 100) && (radio_info_new.cur_freq/1000 < 100))
						redraw = 1;
					break;
			}
		}
		rt_thread_mdelay(50);
	}
}

static void _oled_display_thread_entry(void *parameter)
{
	char freq[10];
	
	rt_kprintf("%s\n", __func__);

	OLED_Clear();
	OLED_DrawBMP(0,0,128,8,BMP1);	//图片显示
	OLED_ShowCHinese(25,0,0);//收
	OLED_ShowCHinese(55,0,1);//音
	OLED_ShowCHinese(85,0,2);//机
	rt_thread_mdelay(200);
	/* main loop... */
	while(1)
	{
		get_radio_sys_frequency(&radio_info);
		sprintf(freq, "%d.%dMHz", radio_info.cur_freq/1000, (radio_info.cur_freq%1000)/100);

		if((redraw == 1) && (radio_info.cur_freq/1000 < 100))
		{
			OLED_Clear();
			OLED_DrawBMP(0,0,128,8,BMP1);	//图片显示
			OLED_ShowCHinese(25,0,0);//收
			OLED_ShowCHinese(55,0,1);//音
			OLED_ShowCHinese(85,0,2);//机
			redraw = 0;
		}
		OLED_ShowString(56,4,(char *)freq);  
		rt_thread_mdelay(500);
	}
}

int main(void)
{
	/* init */
	_power_on_init();
	/* create thread */
	_create_thread();
	/* beep on */
	_beep_power_on_prompt();

	/* main loop */
	while(1)
	{
		rt_thread_mdelay(1000);
	}
}
