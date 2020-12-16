#ifndef _BSP_TIM_H
#define _BSP_TIM_H
#include "stm32f10x_conf.h"

/********************������ʱ�� TIM �������壬ֻ�� TIM6��7************/
#define BASIC_TIM6 // ���ʹ�� TIM7��ע�͵�����꼴��

#ifdef BASIC_TIM6 // ʹ�û�����ʱ�� TIM6
#define BASIC_TIM 								TIM6
#define BASIC_TIM_APBxClock_FUN 	RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK 						RCC_APB1Periph_TIM6
#define BASIC_TIM_IRQ 						TIM6_IRQn
#define BASIC_TIM_IRQHandler 			TIM6_IRQHandler

#else // ʹ�û�����ʱ�� TIM7
#define BASIC_TIM 								TIM7
#define BASIC_TIM_APBxClock_FUN 	RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK 						RCC_APB1Periph_TIM7
#define BASIC_TIM_IRQ 						TIM7_IRQn
#define BASIC_TIM_IRQHandler 			TIM7_IRQHandler
#endif

void delay_init(void);

void delay_us(uint32_t us_cnt);

#define delay_ms(x) delay_us(1000*x)	 //��λms

#endif
