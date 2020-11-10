



#include <avr/io.h>

#include "english_6x8_pixel.h"
#include "delay_function.C"

//  Mega48/8  port B pinout for LCD.
#define LCD_DC                 0x01  //  PB0
#define LCD_CE                 0x04  //  PB2
#define SPI_MOSI               0x08  //  PB3
#define LCD_RST                0x10  //  PB4
#define SPI_CLK                0x20  //  PB5

void LCD_init(void);

void LCD_clear(void);

void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);

void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                   unsigned char ch_with,unsigned char num,
                   unsigned char line,unsigned char row);                 

void LCD_write_char(unsigned char c);

void LCD_write_byte(unsigned char data, unsigned char dc);

void delay_1us(void);                 
void delay_nus(unsigned int n);      
void delay_1ms(void);                
void delay_nms(unsigned int n);     
/*-----------------------------------------------------------------------
LCD_init          : 3310LCD初始化

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_init(void)
  {
    PORTB &= ~LCD_RST;          // 产生一个让LCD复位的低电平脉冲
    delay_1us();
    PORTB |= LCD_RST;
    
    PORTB &= ~LCD_CE ;		// 关闭LCD
    delay_1us();
    PORTB |= LCD_CE;		// 使能LCD
    delay_1us();

    LCD_write_byte(0x21, 0);	// 使用扩展命令设置LCD模式
    LCD_write_byte(0xc8, 0);	// 设置偏置电压
    LCD_write_byte(0x06, 0);	// 温度校正
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// 使用基本命令
    LCD_clear();	        // 清屏
    LCD_write_byte(0x0c, 0);	// 设定显示模式，正常显示
        
    PORTB &= ~LCD_CE ;          // 关闭LCD
  }

/*-----------------------------------------------------------------------
LCD_clear         : LCD清屏函数

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_clear(void)
{
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
      LCD_write_byte(0, 1);			
}

/*-----------------------------------------------------------------------
LCD_set_XY        : 设置LCD坐标函数

输入参数：X       ：0－83
          Y       ：0－5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
{
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
}

/*-----------------------------------------------------------------------
LCD_write_char    : 显示英文字符

输入参数：c       ：显示的字符；

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
{
    unsigned int line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
}


void LCD_write_inverse_char(unsigned char c)
{
    unsigned int line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(~font6x8[c][line], 1);
}

/*-----------------------------------------------------------------------
LCD_write_english_String  : 英文字符串显示函数

输入参数：*s      ：英文字符串指针；
          X、Y    : 显示字符串的位置,x 0-83 ,y 0-5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 		
-----------------------------------------------------------------------*/
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s) 
    {
		LCD_write_char(*s);
		s++;
    }
  }

void LCD_write_inverse_string(unsigned char X,unsigned char Y,char *s)
{
    LCD_set_XY(X,Y);
    while (*s) 
    {
		LCD_write_inverse_char(*s);
		s++;
    }
}

/*-----------------------------------------------------------------------
LCD_write_byte    : 使用SPI接口写数据到LCD

输入参数：data    ：写入的数据；
          command ：写数据/命令选择；

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-13 
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char data, unsigned char command)
{
    PORTB &= ~LCD_CE ;		        // 使能LCD
    
    if (command == 0)
      PORTB &= ~LCD_DC ;	        // 传送命令
    else
      PORTB |= LCD_DC ;		        // 传送数据

    SPDR = data;			// 传送数据到SPI寄存器

    while ((SPSR & 0x80) == 0);         // 等待数据传送完毕
	
    PORTB |= LCD_CE ;			// 关闭LCD
}

void lcd_inverse(void)
{
	LCD_write_byte(0b00001101,0);
}
void lcd_no_inverse(void)
{
	LCD_write_byte(0b00001100,0);
}