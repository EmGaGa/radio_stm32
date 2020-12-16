#ifndef _BSP_TIM_H
#define _BSP_TIM_H
#include "stm32f10x_conf.h"

/********************基本定时器 TIM 参数定义，只限 TIM6、7************/
#define BASIC_TIM6 // 如果使用 TIM7，注释掉这个宏即可

#ifdef BASIC_TIM6 // 使用基本定时器 TIM6
#define BASIC_TIM 								TIM6
#define BASIC_TIM_APBxClock_FUN 	RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK 						RCC_APB1Periph_TIM6
#define BASIC_TIM_IRQ 						TIM6_IRQn
#define BASIC_TIM_IRQHandler 			TIM6_IRQHandler

#else // 使用基本定时器 TIM7
#define BASIC_TIM 								TIM7
#define BASIC_TIM_APBxClock_FUN 	RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK 						RCC_APB1Periph_TIM7
#define BASIC_TIM_IRQ 						TIM7_IRQn
#define BASIC_TIM_IRQHandler 			TIM7_IRQHandler
#endif

void delay_init(void);

void delay_us(uint32_t us_cnt);

#define delay_ms(x) delay_us(1000*x)	 //单位ms

#endif
