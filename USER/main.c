#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "modbus_timer.h"
#include "rs485.h"
#include "modbus.h"
#include "read_in.h"
/************************************************************************************
************************************************************************************
		@接线说明：

				USART1_TX(PA9)	->
				USART1_RX(PA10)	->
				USART3_TX(PB10)	->
				USART3_RX(PB11)	->
		@author：漫步青云 
		@Date:	 2022/2/12 
		@WX:		 15518404326
************************************************************************************
************************************************************************************/
int main(void)
{
	NVIC_Configuration(); 	 				//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	    	 						//延时函数初始化	  
	uart_init(9600);	 							//串口1初始化为9600（只用来打印测试数据）	
	LED_Init();			     						//LED端口初始化
	READ_IN_Init();									//外部信号输入端口初始化(PA1)
	//KEY_Init();          					//初始化与按键连接的硬件接口
	Modbus_uart3_init(9600);				//初始化modbus串口2和485控制引脚
	Modbus_TIME3_Init(7200-1,10-1);	//定时器初始化参数1是重装载数，参数2是分频系数//1ms中断一次
	Modbus_Init();									//MODBUS初始化--本机做作为从机设备地址，本机要匹配的从机地址
	while(1)
	{
		if(READ_IN==1)
		{
			Host_Read03_slave(0x01,0x0007,0x0002);//参数2起始地址，参数3寄存器个数
			if(modbus.Host_send_flag)
			{
				
				LED1=~LED1;
				modbus.Host_Sendtime=0;		//发送完毕后计数清零（距离上次的时间）
				modbus.Host_time_flag=0;	//发送数据标志位清零
				modbus.Host_send_flag=0;	//清空发送结束数据标志位			
				HOST_ModbusRX();					//接收数据进行处理				
			}
			LED0=~LED0;
			delay_ms(1000);	
		}
		else
		{
			LED0=0;
		}	
	}			
}


