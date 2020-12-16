#include "rd5767_head.h"

void delay_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	// ������ʱ��ʱ��,���ڲ�ʱ�� CK_INT=72M
	BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);
	
	// �Զ���װ�ؼĴ����ܵ�ֵ(����ֵ)
	TIM_TimeBaseInitStruct.TIM_Period=1;
	// ʱ��Ԥ��Ƶ��Ϊ 71����������������ʱ�� CK_CNT = CK_INT / (71+1)=1M
	TIM_TimeBaseInitStruct.TIM_Prescaler= 71;
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseInitStruct);
	
	// ʹ�ܼ�����
	TIM_Cmd(BASIC_TIM, DISABLE);
	
	// ��ʱ�رն�ʱ����ʱ�ӣ��ȴ�ʹ��
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
