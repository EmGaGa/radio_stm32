一、如何给显示屏旋转方向
  在spi.h文件中定义了一个DERECTION宏，通过给宏赋值 0.1.2.3这四个值俩修改显示方向
  
二、自定义汉字显示
    本例程的ASCII码和汉字取模方式均为 逐行式  阴码 逆向  C51格式
	如果用户想显示自定义汉字，可以调用函数
	void LCD_ShowFlexibleChinese(u16 x,u16 y,u8 index,u8 sizex,u8 sizey,u16 color);
三、含有两位小数变量的显示
    调用函数void LCD_ShowNum1(u16 x,u16 y,float num,u8 len,u16 color);