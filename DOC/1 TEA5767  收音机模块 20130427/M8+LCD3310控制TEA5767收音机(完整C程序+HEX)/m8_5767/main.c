// WINAVR    GCC
// ATmega8     
// clock: internal 1Mhz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <avr/eeprom.h>
#include "3310LCD_function.c"

#define uchar uint8_t
#define uint  uint16_t

#define SLA_W 0b11000000
#define SLA_R 0b11000001

uchar senddata[5] ;
uchar readdata[5] ;
uchar search = 0;
uchar search_up = 0;

//-------------------------------
// 延时
void delay_ms(uint ms)
{
	uint i;
	for(i=0;i<ms;i++)
		_delay_loop_2(250);
}

void set5767(void)
{
	uchar i = 0;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // SEND START SIGNAL
	while (!(TWCR & (1<<TWINT)));           // WAIT FOR START SIG
	//if ((TWSR & 0xF8) != START)  ERROR();
	
	TWDR = SLA_W;                            // send address
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//if ((TWSR & 0xF8) !=MT_SLA_ACK) ERROR();
	
	for ( i = 0; i < 5; i++ )
	{
		TWDR = senddata[i];//save版本没有[i]
		TWCR = (1<<TWINT) | (1<<TWEN);      // send data
		while (!(TWCR & (1<<TWINT)));
	//	if ((TWSR & 0xF8) != MT_DATA_ACK) ERROR();
	}
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL
	
	//LCD_write_english_string(60,3,"sOK");
}
 
 
void read5767(void)
{
	uchar i = 0;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); // SEND START SIGNAL
	while (!(TWCR & (1<<TWINT)));           // WAIT FOR START SIG
	//if ((TWSR & 0xF8) != START)  ERROR();
	
	TWDR = SLA_R;                            // send address
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	//if ((TWSR & 0xF8) !=MT_SLA_ACK) ERROR();
	//TWCR = (1<<TWINT) | (1<<TWEN);
	
	for ( i = 0; i < 5; i++ )
	{
		TWCR = (1<<TWINT) | (1<<TWEN);      // read data
		while (!(TWCR & (1<<TWINT)));
		readdata[i] = TWDR ;//save版本没有[i]
		
		
	//	if ((TWSR & 0xF8) != MT_DATA_ACK) ERROR();
	}
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //SEND STOP SIGNAL

	//LCD_write_english_string(10,3,"rOK");
}

void show_frequency(void)		
{
	uint32_t nPLL =0;			    //Dec 
	uint32_t frequency = 0;	        //Khz
	uchar display_bit[5];
	
	uchar tbTmp1=readdata[1];
	uchar tbTmp2=readdata[0];
	tbTmp2&=0x3f;

	nPLL=tbTmp2*256+tbTmp1;//save版本:nPLL=pll ;//tbTmp2*256+tbTmp1;
          
	// calculate searched station frequency  
    if(senddata[2]&0x10)
		frequency =(unsigned long)(nPLL*82/10-225);
    else
		frequency =(unsigned long)(nPLL*82/10+225);
		
	display_bit[0] = frequency / 100000 ;
	if ( display_bit[0] == 0 ) display_bit[0] = 0x20;
	else display_bit[0] += 0x30;
	
	display_bit[1] = (frequency / 10000)%10 +0x30;
	display_bit[2] = (frequency / 1000)%10 +0x30;
	display_bit[3] = (frequency / 100)%10 +0x30;
	display_bit[4] = (frequency / 10)%10 +0x30;
	LCD_write_english_string(0,2," " );
	 LCD_write_char(display_bit[0]);
	 LCD_write_char(display_bit[1]);
	 LCD_write_char(display_bit[2]);
	 LCD_write_english_string(25,2,"." );
	 LCD_write_char(display_bit[3]);
	 LCD_write_char(display_bit[4]);
	LCD_write_english_string(43,2," MHz" );
   
}
void show_rx_power(void)
{
	uchar temp;
	
	temp = (readdata[3]>>4);
	
	LCD_write_english_string(60,0," ");
	
	LCD_write_char((temp/10)%10 + 0x30 );
	LCD_write_char(temp%10 + 0x30 );
}

void init(void)
{
	DDRB = 0XFF;
	PORTB = 0XFF;
	
	DDRD = 0B11100000;
	PORTD = 0XFF;
	
	DDRC = 0B00000000;
	PORTC = 0Xff;      //  IO init
	
	delay_nms(100);
	//TWI
	TWBR = 12;
	TWCR = (1<<TWEN); //SEND STOP SIGNAL
	
	 
    OSCCAL=0x9d;                        // 8M系统内部时钟校准    
    //设置MCU的I/O口
    DDRB |= LCD_RST | LCD_DC | LCD_CE | SPI_MOSI | SPI_CLK;
    
	SPSR |= (1<<SPI2X);                 // 设置SPI时钟倍速
    SPCR |= (1<<SPE)|(1<<MSTR);		// 使能SPI接口，主机模式，4M时钟
 
    LCD_init(); //初始化液晶  

}


//////////////////////////////////////////////////////////////////
int main(void)
{
	init();
	
	senddata[0] = 0b01101111;  //load 100MHz pll
	senddata[1] = 0b10010011;  // away's low side injection
	senddata[2] = 0b00100000;
	senddata[3] = 0b10010000;                                                                  
	senddata[4] = 0b00000000;
	readdata[0] = senddata[0];
	readdata[1] = senddata[1];
	
	LCD_write_english_string(0,0,"FM STEREO" );	
	//LCD_write_english_string(0,5,"  stereo" );
	LCD_write_english_string(0,2,"  99.1Mhz" );
	show_frequency();
	LCD_write_inverse_string(0,5,"  UP  ");
	LCD_write_inverse_string(48,5," DOWN ");
	
	while(1)
	{
	
		if ( bit_is_clear( PIND,3))
		{
			delay_nms(20);
			if ( bit_is_clear( PIND,3))
			{
				loop_until_bit_is_set(PIND, 3);
				LCD_write_english_string(0,4,"KEY1 press" );
				senddata[2] &= 0x7f;   // set search down
				set5767();
				LCD_write_english_string(0,4,"searching down" );
				
				search_up = 0;
				search = 1;
			}
		}
		
		if ( bit_is_clear( PIND,2))
		{
			delay_nms(20);
			if ( bit_is_clear( PIND,2))
			{
				
				loop_until_bit_is_set(PIND, 2);
				LCD_write_english_string(0,4,"KEY2 press" );
				
				senddata[2] |= 0x80;   // set search up
				set5767();
				LCD_write_english_string(0,4,"searching up  ");
				
				search_up = 1;
				search = 1;
				
			}
		}
		
		
		if ( search )
		{
			read5767();
			
			if ( readdata[0] & 0x40 )  //reach band end
			{
				if ( search_up )
				{
					senddata[0] = 0b01101001;    // load 88Mhz pll
					senddata[1] = 0b11011010;
					set5767();
				}
				else                  
				{
					senddata[0] = 0b01110011;    // load 108Mhz pll
					senddata[1] = 0b01100100;
					set5767();
				}
			}
			
			
			
			if ( readdata[0] & 0x80)   //has been found a station
			{
				show_frequency();
				show_rx_power();
				senddata[0] = ((readdata[0] & 0x3f) |0x40);
				senddata[1] = readdata[1];  //record the pll
				
				search = 0;
			}
			
		}
		
	}
}
