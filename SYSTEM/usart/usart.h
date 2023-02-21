#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����


extern u8 table_data[9];//������ǰ����һ�������Ž��յ�������
extern u8 table_cp[9];//���Ƕ��ⶨ��һ�����飬�����յ������ݸ��Ƶ�������
extern u16 count;//�������ݼ���

//���ڳ�ʼ������
void uart_init(u32 bound);//��ڲ����ǲ�����

//���ڷ���һ���ֽڵ����ݺ�����һ���ֽ���8λ���ݣ�����Ĳ���������һ��8λ�ģ�
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data);

//���������ֽ����ݺ���
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data);

//����һ����������
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num);

//�����ַ���
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str);//ָ�����ڣ�Ҫ���͵��ַ�������

//int fgetc(FILE *f);
#endif


