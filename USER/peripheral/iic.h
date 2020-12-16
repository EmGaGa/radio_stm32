#ifndef __IIC_H
#define __IIC_H 	
#include "stm32f10x.h"
#include "sys.h"

//IO��������
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO��������	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //����SDA 

//IIC���в�������
void 	IIC_Init(void);                			//��ʼ��IIC��IO��				 
void 	IIC_Start(void);						//����IIC��ʼ�ź�
void 	IIC_Stop(void);	  						//����IICֹͣ�ź�
void 	IIC_Send_Byte(u8 txd);					//IIC����һ���ֽ�
u8		IIC_Read_Byte(unsigned char ack);	 //IIC��ȡһ���ֽ�
u8		IIC_Wait_Ack(void); 					//IIC�ȴ�ACK�ź�
void 	IIC_Ack(void);							//IIC����ACK�ź�
void 	IIC_NAck(void);							//IIC������ACK�ź�

void 	IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8		IIC_Read_One_Byte(u8 daddr,u8 addr);	  

#if 0
/* Ӳ��IIC */
void	I2C1_GPIO_Configuration(void);
void	I2C1_Configuration(void);
void	I2C1_Init(void);
void	I2C1_Write(u8 addr, u8 data);
u8		I2C1_Read(u8 nAddr);
#endif


#endif

