#include "iic.c"
#define max_freq 108000
#define min_freq 87500
unsigned char radio_write_data[5]={0x29,0xc2,0x20,0x11,0x00};        //要写入TEA5767的数据
unsigned char radio_read_data[5];        //TEA5767读出的状态
unsigned int default_pll=0x29c2;//0x29f9;        //默认存台的pll,87.8MHz
//unsigned int max_pll=0x339b;            //108MHz时的pll,
//unsigned int min_pll=9000;            //70MHz时的pll

void radio_write(void)
{
    unsigned char i;
    iic_start();
    iic_write8bit(0xc0);        //TEA5767写地址
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

//读TEA5767状态,并转换成频率
void radio_read(void)
{
    unsigned char i;
    unsigned char temp_l,temp_h;
    pll=0;
    iic_start();
    iic_write8bit(0xc1);        //TEA5767读地址
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

//由频率计算PLL
void get_pll(void)
{
    unsigned char hlsi;
    unsigned int twpll=0;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //频率单位:k
    else
        pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //频率单位:k
}
//由PLL计算频率
void get_frequency(void)
{
    unsigned char hlsi;
    unsigned int npll=0;
    npll=pll;
    hlsi=radio_write_data[2]&0x10;
    if (hlsi)
        frequency=(unsigned long)((float)(npll)*(float)8.192-225);    //频率单位:KHz
    else
        frequency=(unsigned long)((float)(npll)*(float)8.192+225);    //频率单位:KHz
}

//手动设置频率,mode=1,+0.1MHz; mode=0:-0.1MHz ,不用考虑TEA5767用于搜台的相关位:SM,SUD
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

//自动搜台,mode=1,频率增加搜台; mode=0:频率减小搜台,不过这个好像不能循环搜台
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
    while(!(radio_read_data[0]&0x80))     //搜台成功标志
    {
        radio_read();
        disp_freq(9,1);
    }    
}
