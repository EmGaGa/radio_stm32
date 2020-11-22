//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : OLED 4接口演示例程(51系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC   接5V或3.3v电源
//              CLK   接PG12（SCL）
//              MOSI  接PD5（SDA）
//              RES   接PD4
//              DC    接PD15 
//							CS    接D1 
//							BLK   可以不接
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/
#include "delay.h"
#include "sys.h"
#include "oled.h"

int main(void)
 {	
	 u8 i,m;
	 float t=0;
	 delay_init(168);	    	 //延时函数初始化	  
	 Lcd_Init();			//初始化OLED  
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;
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
}