#ifndef __MODBUS_TIMER_H
#define __MODBUS_TIMER_H	 
#include "sys.h"



//������Ҫʶ�����һ������
//��ͷ����β
//���հ����֮���ʱ���������ж�û�н��յ��µ����ݣ�������Ϊ���ݽ������

extern u8 sec_flag;
extern int time;
void Modbus_TIME3_Init(u16 arr,u16 psc);
		 				    
#endif



