//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : OLED 4�ӿ���ʾ����(51ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC   ��5V��3.3v��Դ
//              CLK   ��PG12��SCL��
//              MOSI  ��PD5��SDA��
//              RES   ��PD4
//              DC    ��PD15 
//							CS    ��D1 
//							BLK   ���Բ���
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//******************************************************************************/
#include "delay.h"
#include "sys.h"
#include "oled.h"

int main(void)
 {	
	 u8 i,m;
	 float t=0;
	 delay_init(168);	    	 //��ʱ������ʼ��	  
	 Lcd_Init();			//��ʼ��OLED  
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;
	 while(1)
	 {
		 	LCD_ShowChinese(28,0,0,32,RED);   //��
			LCD_ShowChinese(60,0,1,32,RED);   //��
			LCD_ShowChinese(92,0,2,32,RED);   //԰
		 
		 	LCD_ShowChinese(20,40,0,16,RED);   //��
			LCD_ShowChinese(36,40,1,16,RED);   //��
			LCD_ShowChinese(52,40,2,16,RED);   //԰
			LCD_ShowChinese(68,40,3,16,RED);  //��
			LCD_ShowChinese(84,40,4,16,RED);  //��
		  LCD_ShowChinese(100,40,5,16,RED);  //��
		  LCD_ShowChinese(116,40,6,16,RED);  //��
		  LCD_ShowString(0,60,"0.96 TFT SPI",RED);
			LCD_ShowNum1(104,60,t,5,RED);//��ʾС��
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