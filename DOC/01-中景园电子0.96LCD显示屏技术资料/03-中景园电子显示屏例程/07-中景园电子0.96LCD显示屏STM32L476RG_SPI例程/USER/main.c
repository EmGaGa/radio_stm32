//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2018-10-31
//  ����޸�   : 
//  ��������   : STM32L476RGT6������
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              SCL   ��PA5
//              SDA   ��PA7
//              RES   ��PD2
//              DC    ��PB5
//              CS    ��PA4
//              BLK   ��PB6�����߲��ӣ�       
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2018-10-31
//All rights reserved
#include "main.h"
#include "stm32l4xx_hal.h"
#include "lcd.h"
void SystemClock_Config(void);
int main()
{
	float t=0;
	u8 i;
	HAL_Init();
	SystemClock_Config();
	 Lcd_Init();			//��ʼ��OLED  
	 LCD_Clear(WHITE);
	 BACK_COLOR=WHITE;
	 LED_ON;
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
			HAL_Delay(1000);
		  LCD_Clear(WHITE);

			LCD_ShowString(10,0,"LCD_W:",RED);	LCD_ShowNum(70,0,LCD_W,3,RED);
			LCD_ShowString(10,20,"LCD_H:",RED);LCD_ShowNum(70,20,LCD_H,2,RED);
			for(i=0;i<4;i++)
			{
				LCD_ShowPicture(i*40,40,39+i*40,79);
			}
			HAL_Delay(1000);
		  LCD_Clear(WHITE);
	}
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
		
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {

  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {

  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}











