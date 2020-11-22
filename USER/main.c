#include "rd5767_head.h"
#include <rtthread.h>
#include "lcd.h"
int main(void)
{	
	#if 0
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
	#else
	 u8 i,m;
	 float t=0;
	 delay_init();	    	 //延时函数初始化	  
	 Lcd_Init();			//初始化OLED  
//	 LED_Init();
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;
	 LED0=0;
	 while(1)
	 {
		 	LCD_ShowChinese(28,0,0,32,RED);   //中
			LCD_ShowChinese(60,0,1,32,RED);   //景
			LCD_ShowChinese(92,0,2,32,RED);   //园
		 
		 	LCD_ShowChinese(20,40,0,16,RED);   //中
			LCD_ShowChinese(36,40,1,16,RED);   //景
			LCD_ShowChinese(52,40,2,16,RED);   //园
			LCD_ShowChinese(68,40,3,16,RED);  //电
			LCD_ShowChinese(84,40,4,16,RED);  //子
		  LCD_ShowChinese(100,40,5,16,RED);  //科
		  LCD_ShowChinese(116,40,6,16,RED);  //技
		  LCD_ShowString(0,60,"0.96 TFT SPI",RED);
			LCD_ShowNum1(104,60,t,5,RED);//显示小数
			t+=0.01;
			delay_ms(1000);
		  LCD_Clear(WHITE);

			LCD_ShowString(10,0,"LCD_W:",RED);	LCD_ShowNum(70,0,LCD_W,3,RED);
			LCD_ShowString(10,20,"LCD_H:",RED);LCD_ShowNum(70,20,LCD_H,2,RED);
			for(i=0;i<4;i++)
			{
				LCD_ShowPicture(i*40,40,39+i*40,79);
			}
			delay_ms(1000);
		  LCD_Clear(WHITE);
   }
	#endif
}
