



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
LCD_init          : 3310LCD��ʼ��

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_init(void)
  {
    PORTB &= ~LCD_RST;          // ����һ����LCD��λ�ĵ͵�ƽ����
    delay_1us();
    PORTB |= LCD_RST;
    
    PORTB &= ~LCD_CE ;		// �ر�LCD
    delay_1us();
    PORTB |= LCD_CE;		// ʹ��LCD
    delay_1us();

    LCD_write_byte(0x21, 0);	// ʹ����չ��������LCDģʽ
    LCD_write_byte(0xc8, 0);	// ����ƫ�õ�ѹ
    LCD_write_byte(0x06, 0);	// �¶�У��
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// ʹ�û�������
    LCD_clear();	        // ����
    LCD_write_byte(0x0c, 0);	// �趨��ʾģʽ��������ʾ
        
    PORTB &= ~LCD_CE ;          // �ر�LCD
  }

/*-----------------------------------------------------------------------
LCD_clear         : LCD��������

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
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
LCD_set_XY        : ����LCD���꺯��

���������X       ��0��83
          Y       ��0��5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
{
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
}

/*-----------------------------------------------------------------------
LCD_write_char    : ��ʾӢ���ַ�

���������c       ����ʾ���ַ���

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
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
LCD_write_english_String  : Ӣ���ַ�����ʾ����

���������*s      ��Ӣ���ַ���ָ�룻
          X��Y    : ��ʾ�ַ�����λ��,x 0-83 ,y 0-5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 		
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
LCD_write_byte    : ʹ��SPI�ӿ�д���ݵ�LCD

���������data    ��д������ݣ�
          command ��д����/����ѡ��

��д����          ��2004-8-10 
����޸�����      ��2004-8-13 
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char data, unsigned char command)
{
    PORTB &= ~LCD_CE ;		        // ʹ��LCD
    
    if (command == 0)
      PORTB &= ~LCD_DC ;	        // ��������
    else
      PORTB |= LCD_DC ;		        // ��������

    SPDR = data;			// �������ݵ�SPI�Ĵ���

    while ((SPSR & 0x80) == 0);         // �ȴ����ݴ������
	
    PORTB |= LCD_CE ;			// �ر�LCD
}

void lcd_inverse(void)
{
	LCD_write_byte(0b00001101,0);
}
void lcd_no_inverse(void)
{
	LCD_write_byte(0b00001100,0);
}