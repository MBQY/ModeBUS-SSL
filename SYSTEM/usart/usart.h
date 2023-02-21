#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收


extern u8 table_data[9];//这是提前定义一个数组存放接收到的数据
extern u8 table_cp[9];//这是额外定义一个数组，将接收到的数据复制到这里面
extern u16 count;//接收数据计数

//串口初始化函数
void uart_init(u32 bound);//入口参数是波特率

//串口发送一个字节的数据函数（一个字节是8位数据，定义的参数变量是一个8位的）
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data);

//发送两个字节数据函数
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data);

//发送一个数组数据
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);

//发送字符串
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str);//指定串口，要发送的字符串内容

//int fgetc(FILE *f);
#endif


