#ifndef _modbus_h_
#define _modbus_h_
#include "stm32f10x.h"
#include "modbus_crc.h"
#include "modbus_timer.h"
#include "rs485.h"


typedef struct 
{
	//作为从机时使用
  u8  myadd;        //本设备从机地址
	u8  rcbuf[100];   //modbus接受缓冲区
	u8  timout;       //modbus数据持续时间
	u8  recount;      //modbus端口接收到的数据个数
	u8  timrun;       //modbus定时器是否计时标志
	u8  reflag;       //modbus一帧数据接受完成标志位
	u8  sendbuf[100]; //modbus接发送缓冲区
	
	//作为主机添加部分
	u8 Host_Txbuf[8];	//modbus发送数组	
	u8 slave_add;		//要匹配的从机设备地址（做主机实验时使用）
	u8 Host_send_flag;//主机设备发送数据完毕标志位
	int Host_Sendtime;//发送完一帧数据后时间计数
	u8 Host_time_flag;//发送时间到标志位，=1表示到发送数据时间了
	u8 Host_End;//接收数据后处理完毕
}MODBUS;


//typedef struct  
//{
//    u8 addr;//从机地址
//    u8 start;//寄存器起始
//    u8 len;  //接收到或待发送的寄存器数
//    u8 flag;
//    u8 buf[100];//寄存器数据

////}Host_slave; //用户数据

//extern Host_slave Modbus_TX;
//extern Host_slave Modbus_RX;


extern MODBUS modbus;
extern u16 Reg[];
void Modbus_Init(void);
void Modbus_Func3(void);//读寄存器数据
void Modbus_Func6(void);//往1个寄存器中写入数据
void Modbus_Func16(void);//往多个寄存器中写入数据
void Modbus_Event(void);



//void Host_send03(void);
void Host_Read03_slave(uint8_t slave,uint16_t StartAddr,uint16_t num);
void Host_RX(void);
//主机接收从机的消息进行处理
void HOST_ModbusRX(void);
void RS485_Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);

#endif

