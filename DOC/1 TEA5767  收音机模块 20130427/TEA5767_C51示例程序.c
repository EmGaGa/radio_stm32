#include "iic.c"
#define max_freq 108000
#define min_freq 87500
unsigned char radio_write_data[5]={0x29,0xc2,0x20,0x11,0x00};        //Ҫд��TEA5767������
unsigned char radio_read_data[5];        //TEA5767������״̬
unsigned int default_pll=0x29c2;//0x29f9;        //Ĭ�ϴ�̨��pll,87.8MHz
//unsigned int max_pll=0x339b;            //108MHzʱ��pll,
//unsigned int min_pll=9000;            //70MHzʱ��pll

void radio_write(void)
{
    unsigned char i;
    iic_start();
    iic_write8bit(0xc0);        //TEA5767д��ַ
    if(!iic_testack())
    {
        for(i=0;i<5;i++)
        {
            iic_write8bit(radio_write_data[i]);
            iic_ack();
        }
    }
    iic_stop();    
}

//��TEA5767״̬,��ת����Ƶ��
void radio_read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    iic_start();
    iic_write8bit(0xc1);        //TEA5767����ַ
    if(!iic_testack())
    {
        for(i=0;i<5;i++)
        {
            radio_read_data[i]=iic_read8bit();
            iic_ack();
        }
    }
    iic_stop();
    temp_l=radio_read_data[1];
    temp_h=radio_read_data[0];
    temp_h&=0x3f;
    pll=temp_h*256+temp_l;
    get_frequency();
}

//��Ƶ�ʼ���PLL
void get_pll(void)
{
    unsigned char hlsi;
    unsigned int twpll=0;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
    else
        pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
}
//��PLL����Ƶ��
void get_frequency(void)
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

//�ֶ�����Ƶ��,mode=1,+0.1MHz; mode=0:-0.1MHz ,���ÿ���TEA5767������̨�����λ:SM,SUD
void search(bit mode)
{
    radio_read();        
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
    get_pll();
    radio_write_data[0]=pll/256;
    radio_write_data[1]=pll%256;
    radio_write_data[2]=0x20;
    radio_write_data[3]=0x11;
    radio_write_data[4]=0x00;
    radio_write();
}

//�Զ���̨,mode=1,Ƶ��������̨; mode=0:Ƶ�ʼ�С��̨,�������������ѭ����̨
void auto_search(bit mode)
{
    radio_read();
    get_pll();
    if(mode)
        radio_write_data[2]=0xa0;
    else
        radio_write_data[2]=0x20;    
    radio_write_data[0]=pll/256+0x40;
    radio_write_data[1]=pll%256;    
    radio_write_data[3]=0x11;
    radio_write_data[4]=0x00;
    radio_write();
    radio_read();
    while(!(radio_read_data[0]&0x80))     //��̨�ɹ���־
    {
        radio_read();
        disp_freq(9,1);
    }    
}
