#include "stm32f10x.h"

void Delay(u32 count)
{
	u32 i=0;
	for(;i<count;i++);
}
int main(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|
	RCC_APB2Periph_GPIOE, ENABLE);	    //使能PB,PE端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			    //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			     //初始化GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);					//PB.5 输出高
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	            //LED1-->PE.5推挽输出
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  	       //初始化GPIO
	GPIO_SetBits(GPIOE,GPIO_Pin_5); 			 //PE.5 输出高 	  
	while(1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
		Delay(3000000);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		Delay(3000000);
	}
}
