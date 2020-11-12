#include "tea5767.h"

#define max_freq 108000
#define min_freq 87500
static unsigned char radio_write_data[5]={0x29,0xc2,0x20,0x11,0x00};        //Ҫд��TEA5767������
static unsigned char radio_read_data[5];        //TEA5767������״̬
//static unsigned int default_pll=0x29c2;//0x29f9;        //Ĭ�ϴ�̨��pll,87.8MHz
//unsigned int max_pll=0x339b;            //108MHzʱ��pll,
//unsigned int min_pll=9000;            //70MHzʱ��pll
static unsigned int pll = 0;
static unsigned long frequency = 0;

void tea5767_init(void)
{
	IIC_Init();
}

void tea5767_write(void)
{
    unsigned char i;
    IIC_Start();
    IIC_Send_Byte(0xc0);        //TEA5767д��ַ
    if(!IIC_Wait_Ack())
    {
        for(i=0;i<5;i++)
        {
            IIC_Send_Byte(radio_write_data[i]);
            IIC_Ack();
        }
    }
    IIC_Stop(); 
}

void tea5767_read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    IIC_Start();
    IIC_Send_Byte(0xc1);        //TEA5767����ַ
    if(!IIC_Wait_Ack())
    {
        for(i=0;i<5;i++)
        {
            radio_read_data[i]=IIC_Read_Byte(1);
        }
    }
    IIC_Stop();
    temp_l=radio_read_data[1];
    temp_h=radio_read_data[0];
    temp_h&=0x3f;
    pll=temp_h*256+temp_l;
    get_tea5767_frequency();
}

void get_tea5767_pll(void)
{
    unsigned char hlsi;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
    else
        pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k	
}

void get_tea5767_frequency(void)
{
    unsigned char hlsi;
    unsigned int npll=0;
    npll=pll;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        frequency=(unsigned long)((float)(npll)*(float)8.192-225);    //Ƶ�ʵ�λ:KHz
    else
        frequency=(unsigned long)((float)(npll)*(float)8.192+225);    //Ƶ�ʵ�λ:KHz	
}

void tea5767_search(int mode)
{
    tea5767_read();        
    if(mode)
    {
        frequency+=100;
        if(frequency>max_freq)
            frequency=min_freq;
    }
    else
    {
        frequency-=100;
        if(frequency<min_freq)
            frequency=max_freq;
    }          
    get_tea5767_pll();
    radio_write_data[0]=pll/256;
    radio_write_data[1]=pll%256;
    radio_write_data[2]=0x20;
    radio_write_data[3]=0x11;
    radio_write_data[4]=0x00;
    tea5767_write();	
}

//�Զ���̨,mode=1,Ƶ��������̨; mode=0:Ƶ�ʼ�С��̨,�������������ѭ����̨
void tea5767_auto_search(int mode)
{
    tea5767_read();
    get_tea5767_pll();
    if(mode)
        radio_write_data[2]=0xa0;
    else
        radio_write_data[2]=0x20;    
    radio_write_data[0]=pll/256+0x40;
    radio_write_data[1]=pll%256;    
    radio_write_data[3]=0x11;
    radio_write_data[4]=0x00;
    tea5767_write();
    tea5767_read();
    while(!(radio_read_data[0]&0x80))     //��̨�ɹ���־
    {
        tea5767_read();
    //    disp_freq(9,1);
    }    
}


