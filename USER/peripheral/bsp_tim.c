#include "rd5767_head.h"

void delay_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	// 开启定时器时钟,即内部时钟 CK_INT=72M
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
	
	// 自动重装载寄存器周的值(计数值)
	TIM_TimeBaseInitStruct.TIM_Period=1;
	// 时钟预分频数为 71，则驱动计数器的时钟 CK_CNT = CK_INT / (71+1)=1M
	TIM_TimeBaseInitStruct.TIM_Prescaler= 71;
	// 初始化定时器
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseInitStruct);
	
	// 使能计数器
	TIM_Cmd(BASIC_TIM, DISABLE);
	
	// 暂时关闭定时器的时钟，等待使用
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}

void delay_us(uint32_t us_cnt)
{
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
//	TIM_Cmd(BASIC_TIM, ENABLE);
	BASIC_TIM->CR1 |= TIM_CR1_CEN;
	while(us_cnt--){
		while((BASIC_TIM->SR & TIM_FLAG_Update) == RESET);
		BASIC_TIM->SR &= ~TIM_FLAG_Update;
	}
	BASIC_TIM->CR1 &= ~TIM_CR1_CEN;
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
//	TIM_Cmd(BASIC_TIM, DISABLE);
}
