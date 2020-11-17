/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
 
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>

#include "stm32f10x.h"
#include <string.h>

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called 
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

// Holds the system core clock, which is the system clock 
// frequency supplied to the SysTick timer and the processor 
// core clock.
extern uint32_t SystemCoreClock;

static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }
    
    _SYSTICK_LOAD = ticks - 1; 
    _SYSTICK_PRI = 0xFF;
    _SYSTICK_VAL  = 0;
    _SYSTICK_CTRL = 0x07;  
    
    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

//使用串口中断实现 FinSH

//*************************************** 
//第一部分：ringbuffer 实现部分
//***************************************
#define rt_ringbuffer_space_len(rb) ((rb)->buffer_size - rt_ringbuffer_data_len(rb))

struct rt_ringbuffer
{
    rt_uint8_t *buffer_ptr;

    rt_uint16_t read_mirror : 1;
    rt_uint16_t read_index : 15;
    rt_uint16_t write_mirror : 1;
    rt_uint16_t write_index : 15;

    rt_int16_t buffer_size;
};

enum rt_ringbuffer_state
{
    RT_RINGBUFFER_EMPTY,
    RT_RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    RT_RINGBUFFER_HALFFULL,
};

rt_inline enum rt_ringbuffer_state rt_ringbuffer_status(struct rt_ringbuffer *rb)
{
    if (rb->read_index == rb->write_index)
    {
        if (rb->read_mirror == rb->write_mirror)
            return RT_RINGBUFFER_EMPTY;
        else
            return RT_RINGBUFFER_FULL;
    }
    return RT_RINGBUFFER_HALFFULL;
}

/** 
 * get the size of data in rb 
 */
rt_size_t rt_ringbuffer_data_len(struct rt_ringbuffer *rb)
{
    switch (rt_ringbuffer_status(rb))
    {
    case RT_RINGBUFFER_EMPTY:
        return 0;
    case RT_RINGBUFFER_FULL:
        return rb->buffer_size;
    case RT_RINGBUFFER_HALFFULL:
    default:
        if (rb->write_index > rb->read_index)
            return rb->write_index - rb->read_index;
        else
            return rb->buffer_size - (rb->read_index - rb->write_index);
    };
}

void rt_ringbuffer_init(struct rt_ringbuffer *rb,
                        rt_uint8_t           *pool,
                        rt_int16_t            size)
{
    RT_ASSERT(rb != RT_NULL);
    RT_ASSERT(size > 0);

    /* initialize read and write index */
    rb->read_mirror = rb->read_index = 0;
    rb->write_mirror = rb->write_index = 0;

    /* set buffer pool and size */
    rb->buffer_ptr = pool;
    rb->buffer_size = RT_ALIGN_DOWN(size, RT_ALIGN_SIZE);
}

/**
 * put a character into ring buffer
 */
rt_size_t rt_ringbuffer_putchar(struct rt_ringbuffer *rb, const rt_uint8_t ch)
{
    RT_ASSERT(rb != RT_NULL);

    /* whether has enough space */
    if (!rt_ringbuffer_space_len(rb))
        return 0;

    rb->buffer_ptr[rb->write_index] = ch;

    /* flip mirror */
    if (rb->write_index == rb->buffer_size-1)
    {
        rb->write_mirror = ~rb->write_mirror;
        rb->write_index = 0;
    }
    else
    {
        rb->write_index++;
    }

    return 1;
}

/**
 * get a character from a ringbuffer
 */
rt_size_t rt_ringbuffer_getchar(struct rt_ringbuffer *rb, rt_uint8_t *ch)
{
    RT_ASSERT(rb != RT_NULL);

    /* ringbuffer is empty */
    if (!rt_ringbuffer_data_len(rb))
        return 0;

    /* put character */
    *ch = rb->buffer_ptr[rb->read_index];

    if (rb->read_index == rb->buffer_size-1)
    {
        rb->read_mirror = ~rb->read_mirror;
        rb->read_index = 0;
    }
    else
    {
        rb->read_index++;
    }

    return 1;
}

/* 第二部分：finsh 移植对接部分 */
#define UART_RX_BUF_LEN 16
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
struct rt_ringbuffer  uart_rxcb;         /* 定义一个 ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */
static USART_InitTypeDef UartHandle;

static void uart_init(void)
{
	//GPIO端口设置
  	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 初始化串口接收 ringbuffer  */
    rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	/* 初始化串口接收数据的信号量 */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
    //USART1_RX	  GPIOA.10初始化
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  	//Usart1 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	UartHandle.USART_BaudRate = 115200;//串口波特率
	UartHandle.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	UartHandle.USART_StopBits = USART_StopBits_1;//一个停止位
	UartHandle.USART_Parity = USART_Parity_No;//无奇偶校验位
	UartHandle.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	UartHandle.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  	USART_Init(USART1, &UartHandle); //初始化串口1
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

/************************************************
 rt_hw_console_output的两种实现方法都可以
************************************************/
//方法一
void uart_send_byte(u8 data)
{
	while((USART1->SR & 0X40) == 0);
	USART1->DR = data;
}

void rt_hw_console_output(const char *str)
{
	rt_size_t i = 0, size = 0;
	char  a = '\r';
	
	size = rt_strlen(str);
	for(i = 0; i < size; i++)
	{
		if(*(str + i) == '\n')
		{
			uart_send_byte(a);									//发送数据
		}
		uart_send_byte(*(str+i));									//发送数据
	}
	
}
//方法二
//void rt_hw_console_output(const char *str)
//{
//	rt_size_t i = 0, size = 0;
//	char  a = '\r';
//	
//	size = rt_strlen(str);
//	for(i = 0; i < size; i++)
//	{
//		if(*(str + i) == '\n')
//		{
//			USART_SendData(USART1, a);									//发送数据
//			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//等待发送完成
//		}
//		USART_SendData(USART1, *(str+i));									//发送数据
//		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//等待发送完成
//	}
//	
//}

/************************************************
 rt_hw_console_getchar 
 方法一与方法二是功能相同
 在串口接收命令时候只能一个字符一个字符的接收
 因此使用方法三：
 使用串口中断 把串口接收的命令保存在数组中
************************************************/
//方法一
//char rt_hw_console_getchar()
//{
//	int ch = -1;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//	{
//		ch = USART_ReceiveData(USART1) & 0xff;
//	}
//	else 
//	{
//		if(USART_GetITStatus(USART1, USART_IT_ORE) != RESET)  //
//		{
//			USART_ClearFlag(USART1, USART_FLAG_RXNE);
//		}
//		rt_thread_mdelay(10);
//	}
//	return ch;
//}

//方法二
///* 移植 FinSH，实现命令行交互, 需要添加 FinSH 源码，然后再对接 rt_hw_console_getchar */
///* 查询方式 */
//char rt_hw_console_getchar(void)
//{
//    int ch = -1;
// 
//    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
//    {
//        //USART_ClearITPendingBit(USART_DEBUG,  USART_FLAG_RXNE);
//        ch = USART_ReceiveData(USART1) & 0xFF;
//    }
//    else
//    {
//        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
//        {
//            USART_ClearITPendingBit(USART1,  USART_FLAG_ORE);
//        }
//        rt_thread_mdelay(10);
//    }
//    
//    return ch;
//}

//方法三
/* 移植 FinSH，实现命令行交互, 需要添加 FinSH 源码，然后再对接 rt_hw_console_getchar */
/* 中断方式 */
char rt_hw_console_getchar(void)
{
    char ch = 0;

    /* 从 ringbuffer 中拿出数据 */
    while (rt_ringbuffer_getchar(&uart_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;   
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    int ch = -1;
//    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断

    if ((USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET) &&
        (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
            {
                ch =  USART1->DR & 0xff;
            }
            if (ch == -1)
            {
                break;
            }  
            /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}


/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    /* System Clock Update */
    SystemCoreClockUpdate();
    
    /* System Tick Configuration */
    _SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
	uart_init();
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

