#include "sys.h"
#include "rs485.h"	  
#include "delay.h"
#include "modbus_timer.h"
#include "modbus.h"
/************************************************************************************
************************************************************************************
@说明：

		定时器中定义了Rx_Data_len
		PD7控制485的发送和接收使能
		rs485初始化完毕一般配置为接收使能（正常状态处于接收状态，只有发送的时候才使能发送）

************************************************************************************
************************************************************************************/

//modbus串口发送一个字节数据
void Modbus_Send_Byte(u8 Modbus_byte)
{
  USART_SendData(USART3,Modbus_byte);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	USART_ClearFlag(USART3, USART_FLAG_TC); 
}



//485串口初始化
//初始化IO 串口3
//TX--PB10
//RX--PB11
//bound:波特率
void Modbus_uart3_init(u32 bound){
    //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;//GPIO结构体指针
		USART_InitTypeDef USART_InitStructure;//串口结构体指针
		NVIC_InitTypeDef NVIC_InitStructure;//中断分组结构体指针
		//1、使能串口时钟，串口引脚时钟 串口2挂载到APB1上
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);//使能串口时钟和收发使能时钟
		//2、复位串口	
		USART_DeInit(USART3);  //复位串口1
	
		//3、发送接收引脚的设置
	 //USART3_TX   PB10（由图 可知设置为推挽复用输出）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
   
    //USART3_RX	 PB11（有图可知浮空输入）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
		
		//485收发控制引脚PD7
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PD7
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//普通的推挽输出
    GPIO_Init(GPIOD, &GPIO_InitStructure); //初始化PD7

   //4、USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口
 
   //5、Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
	  //6、开启接收数据中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
		
		//7、使能串口
    USART_Cmd(USART3, ENABLE);                    //使能串口 

		RS485_RX_ENABLE;//使能接收引脚（常态下处于接收状态）
}


//modbus串口中断服务程序
void USART3_IRQHandler(void)                
{
  u8 st,Res;
	st = USART_GetITStatus(USART3, USART_IT_RXNE);
	if(st == SET)//接收中断
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
//	  USART_SendData(USART1, Res);//接受到数据之后返回给串口1
	 if( modbus.reflag==1)  //有数据包正在处理
	  {
		   return ;
		}		
	  modbus.rcbuf[modbus.recount++] = Res;
		modbus.timout = 0;
		if(modbus.recount == 1)  //已经收到了第二个字符数据
		{
		  modbus.timrun = 1;  //开启modbus定时器计时
		}
	}	
} 


