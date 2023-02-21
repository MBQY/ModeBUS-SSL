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
		@����˵����

				USART1_TX(PA9)	->
				USART1_RX(PA10)	->
				USART3_TX(PB10)	->
				USART3_RX(PB11)	->
		@author���������� 
		@Date:	 2022/2/12 
		@WX:		 15518404326
************************************************************************************
************************************************************************************/
int main(void)
{
	NVIC_Configuration(); 	 				//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 						//��ʱ������ʼ��	  
	uart_init(9600);	 							//����1��ʼ��Ϊ9600��ֻ������ӡ�������ݣ�	
	LED_Init();			     						//LED�˿ڳ�ʼ��
	READ_IN_Init();									//�ⲿ�ź�����˿ڳ�ʼ��(PA1)
	//KEY_Init();          					//��ʼ���밴�����ӵ�Ӳ���ӿ�
	Modbus_uart3_init(9600);				//��ʼ��modbus����2��485��������
	Modbus_TIME3_Init(7200-1,10-1);	//��ʱ����ʼ������1����װ����������2�Ƿ�Ƶϵ��//1ms�ж�һ��
	Modbus_Init();									//MODBUS��ʼ��--��������Ϊ�ӻ��豸��ַ������Ҫƥ��Ĵӻ���ַ
	while(1)
	{
		if(READ_IN==1)
		{
			Host_Read03_slave(0x01,0x0007,0x0002);//����2��ʼ��ַ������3�Ĵ�������
			if(modbus.Host_send_flag)
			{
				
				LED1=~LED1;
				modbus.Host_Sendtime=0;		//������Ϻ�������㣨�����ϴε�ʱ�䣩
				modbus.Host_time_flag=0;	//�������ݱ�־λ����
				modbus.Host_send_flag=0;	//��շ��ͽ������ݱ�־λ			
				HOST_ModbusRX();					//�������ݽ��д���				
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


