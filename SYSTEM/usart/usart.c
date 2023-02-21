#include "sys.h"
#include "usart.h"	  
#include "stdio.h"
#include "string.h"
//getchar()�ȼ���scanf()����
//���ʹ��getchar����Ҳ��Ҫ���¶���

////�ض���c�⺯��scanf�����ڣ���д�����ʹ��scanf��getchar����
int fgetc(FILE *f)
{
	//�ȴ�������������
	/* ��������ȴ��Ͳ���Ҫ���ж��н����� */
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	return (int)USART_ReceiveData(USART1);
}
//�������������ʹ��getchar()��Ҫ��������ж����ô���ע�͵���������ͻ

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}

#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}

int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
u8 table_data[9];//������ǰ����һ�������Ž��յ�������
u8 table_cp[9];//���Ƕ��ⶨ��һ�����飬�����յ������ݸ��Ƶ�������
u16 count=0;//�������ݼ���

 
#if EN_USART1_RX   //���ʹ���˽���




//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;//GPIO�ṹ��ָ��
		USART_InitTypeDef USART_InitStructure;//���ڽṹ��ָ��
		NVIC_InitTypeDef NVIC_InitStructure;//�жϷ���ṹ��ָ��
		//1��ʹ�ܴ���ʱ�ӣ���������ʱ�� 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
		
	//2����λ����	
		USART_DeInit(USART1);  //��λ����1
	
	//3�����ͽ������ŵ�����
	 //USART1_TX   PA.9����ͼ ��֪����Ϊ���츴�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10����ͼ��֪�������룩
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


   //4��USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
		
#if EN_USART1_RX		  //���ʹ���˽���  
   //5��Usart1 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
	  //6���������������ж�
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
		
#endif
		//7��ʹ�ܴ���
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}


//uint8_t��8λ�����ݣ�uint16_t��16λ������
//��uint8_t��8λ���ݴ��ݸ�uint16_t��16λ����ʱ�����Զ�ǿ������ת��


//���ڷ���һ���ֽڵ����ݺ�����һ���ֽ���8λ���ݣ�����Ĳ���������һ��8λ�ģ�
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data)//ÿ��ֻ�ܷ���8λ������
{
	//���ù̼��⺯��
	USART_SendData(pUSARTx,data);//��������д������
	
	//�����������Ǽ��TXE���λ�Ƿ���1���������ݼĴ������ˣ������Ѿ������ݴ��ݵ�������λ�Ĵ�����
	//���TXE���λҲ��Ҫһ���̼��⺯��
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
	//������λһֱΪ0�Ļ���һֱ�ȴ���ֻ�е�����ΪSET��Ż��������ѭ������ʾһ���ֽڷ��ͳ�ȥ�ˣ�

}



//��ʱ�򴫸������ݿ�����16λ�ģ���ô���ͣ����������ֽ�
//���������ֽڵ����ݾ���ʮ��λ��
//���ֱ�ʾ16λ�������ֽڣ�������2��ʮ��λ������

//���������ֽ����ݺ���
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data)
{
	//����ʮ��λ����Ҫ��Ϊ���������ͣ��ȶ�����������
	uint8_t temp_h,temp_l;//����8λ�ı������ֱ�洢��8λ�͵�8λ��

	//����ȡ����8λ
	temp_h=(data&0xff00)>>8;//�Ͱ�λ����0��&����8λ��Ϊ0������8λ��0xff00��16λ�����ƣ�
	//��ȡ����8λ
	temp_l=data&0xff;//ȡ����8λ����
	//16λ�����������Ӿͷŵ��������������棨��16λ��
	
	//���ù̼��⺯��
	USART_SendData(pUSARTx,temp_h);//�ȷ��͸�8λ
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//�ȴ����ݷ������

	USART_SendData(pUSARTx,temp_l);//�ٷ��͵�8λ
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//�ȴ����ݷ������

}


//��ʱ�����ݺܶ���Ҫ�������飺����������8λ�� 
//����8λ���ݵ�����--��Ҫдһ��ѭ�������÷���һ���ֽڵĺ�������
//����Ļ��������ľ���һ��ָ����

//����һ����������
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num)
{
	//ÿ����Ҫ���Ͷ������ݣ�ͨ���β�num��������num�������8λ�ģ���ô������෢��255��
	int i;
	for(i=0;i<num;i++)
	{
		//���÷���һ���ֽں����������ݣ���������д�������ԣ�
		//Usart_SendByte(USART1,*array++);
		Usart_SendByte(USART1,array[i]);//ÿ��ֻ�ܷ���8λ����
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//�ȴ��������
}
//�жϷ���һ���ֽڵ����ݱ�־λ��USART_FLAG_TXE
//�жϷ���һ�����ֽڵ����ݱ�־λ��USART_FLAG_TC



//�����ַ���
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str)//ָ�����ڣ�Ҫ���͵��ַ�������
{
	uint8_t i=0;
	//ʹ��do-whileѭ����do��ʱ���Ѿ���ʼ������
	do{
		//��Ҫ���÷���һ���ֽں���
		Usart_SendByte(USART1,*(str+i));//����һ��֮��ָ���ַ����һ��
		i++;
	}while(*(str+i)!='\0');//����β������'\0'Ϊ�棬��������
	//���='\0'��ʾ�������
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//�ȴ��������
}



////�жϷ��������������ַ���ʲô���ݣ��ͻ᷵��ʲô���ݣ�
//void USART1_IRQHandler() 
//{
//	u8 ucTemp;
//	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET)
//	{
//		ucTemp=USART_ReceiveData(USART1);
//		USART_SendData(USART1, ucTemp);
//	}
//}



////ʹ���Զ���Э�����ʮ����������

//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//		u8 Res,i;
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//		{
//			Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//			
//			if(count==0)//����ǽ��յĵ�һ������
//			{
//				table_data[count]=Res;//����һ�����ݴ浽�����е�һԪ��
//				if(table_data[0]==0x2c)//�жϽ��յĵ�һ�������ǲ���ʮ������0X2C
//				  count++;//�����һ��������0X2C���ʾ��ȷ����+1
//			}
//			else if(count==1)//��һ�����ݽ�����ȷ������£��жϵڶ�������
//			{
//				if(Res==0xe4)//����ս��յ�������0xE4���ʾ������ȷ
//				{
//					table_data[count]=Res;//�����ݴ��浽����ڶ���Ԫ��λ��
//					count++;//�������ݼ���+1
//				}
//				else//����ڶ����ַ�����0XE4��������㣬���½���
//					count=0;
//			}
//			else if(count==2&&Res==0)//���ǰ����������ȷ�����յĵ�����������0����������������½�������
//			{
//				count=0;
//			}
//			else if(count>1&&count<9)//���ǿ��Խ������ݵķ�Χ��ֻҪcount�����ݿɽ������ݷ�Χ�ڼ��ɽ��д�������
//			{
//				table_data[count]=Res;
//				count++;
//			}
//			else if(count>=9)//����������ݳ��������С�����������½���
//			{
//				count=0;
//			}		
//   } 
//	 
//		memset(table_cp, 0, sizeof(table_data));//��ʹ������table_cpʱ���
//		for(i=0;i<9;i++)//�ѽ��յ������ݸ��Ƶ�table_cp������
//		{
//			 table_cp[i]= table_data[i];
//		
//	}
//} 



#endif	

