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

//ʹ�ô����ж�ʵ�� FinSH

//*************************************** 
//��һ���֣�ringbuffer ʵ�ֲ���
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

/* �ڶ����֣�finsh ��ֲ�ԽӲ��� */
#define UART_RX_BUF_LEN 16
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
struct rt_ringbuffer  uart_rxcb;         /* ����һ�� ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* ����һ����̬�ź��� */
static USART_InitTypeDef UartHandle;

static void uart_init(void)
{
	//GPIO�˿�����
  	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* ��ʼ�����ڽ��� ringbuffer  */
    rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	/* ��ʼ�����ڽ������ݵ��ź��� */
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
    //USART1_RX	  GPIOA.10��ʼ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  	//Usart1 NVIC ����
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	UartHandle.USART_BaudRate = 115200;//���ڲ�����
	UartHandle.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	UartHandle.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	UartHandle.USART_Parity = USART_Parity_No;//����żУ��λ
	UartHandle.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	UartHandle.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  	USART_Init(USART1, &UartHandle); //��ʼ������1
  	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}

/************************************************
 rt_hw_console_output������ʵ�ַ���������
************************************************/
//����һ
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
			uart_send_byte(a);									//��������
		}
		uart_send_byte(*(str+i));									//��������
	}
	
}
//������
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
//			USART_SendData(USART1, a);									//��������
//			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//�ȴ��������
//		}
//		USART_SendData(USART1, *(str+i));									//��������
//		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//�ȴ��������
//	}
//	
//}

/************************************************
 rt_hw_console_getchar 
 ����һ�뷽�����ǹ�����ͬ
 �ڴ��ڽ�������ʱ��ֻ��һ���ַ�һ���ַ��Ľ���
 ���ʹ�÷�������
 ʹ�ô����ж� �Ѵ��ڽ��յ��������������
************************************************/
//����һ
//char rt_hw_console_getchar()
//{
//	int ch = -1;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
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

//������
///* ��ֲ FinSH��ʵ�������н���, ��Ҫ��� FinSH Դ�룬Ȼ���ٶԽ� rt_hw_console_getchar */
///* ��ѯ��ʽ */
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

//������
/* ��ֲ FinSH��ʵ�������н���, ��Ҫ��� FinSH Դ�룬Ȼ���ٶԽ� rt_hw_console_getchar */
/* �жϷ�ʽ */
char rt_hw_console_getchar(void)
{
    char ch = 0;

    /* �� ringbuffer ���ó����� */
    while (rt_ringbuffer_getchar(&uart_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;   
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
    int ch = -1;
//    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //���ж���һ��Ҫ������Ժ����������ж�

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
            /* ��ȡ�����ݣ������ݴ��� ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //���ж���һ��Ҫ������Ժ������뿪�ж�
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

