#include "sys.h"
#include "usart.h"	  
#include "stdio.h"
#include "string.h"
//getchar()等价于scanf()函数
//如果使用getchar函数也需要重新定义

////重定向c库函数scanf到串口，重写后可以使用scanf和getchar函数
int fgetc(FILE *f)
{
	//等待串口输入数据
	/* 有了这个等待就不需要在中断中进行了 */
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==RESET);
	return (int)USART_ReceiveData(USART1);
}
//如果在主函数中使用getchar()需要把下面的中断设置代码注释掉，否则会冲突

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}

#endif 

/*使用microLib的方法*/
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
 
u8 table_data[9];//这是提前定义一个数组存放接收到的数据
u8 table_cp[9];//这是额外定义一个数组，将接收到的数据复制到这里面
u16 count=0;//接收数据计数

 
#if EN_USART1_RX   //如果使能了接收




//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
    //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;//GPIO结构体指针
		USART_InitTypeDef USART_InitStructure;//串口结构体指针
		NVIC_InitTypeDef NVIC_InitStructure;//中断分组结构体指针
		//1、使能串口时钟，串口引脚时钟 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
		
	//2、复位串口	
		USART_DeInit(USART1);  //复位串口1
	
	//3、发送接收引脚的设置
	 //USART1_TX   PA.9（由图 可知设置为推挽复用输出）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10（有图可知浮空输入）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10


   //4、USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
		
#if EN_USART1_RX		  //如果使能了接收  
   //5、Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
	  //6、开启接收数据中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
		
#endif
		//7、使能串口
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}


//uint8_t是8位的数据，uint16_t是16位的数据
//当uint8_t的8位数据传递给uint16_t的16位数据时，会自动强制类型转换


//串口发送一个字节的数据函数（一个字节是8位数据，定义的参数变量是一个8位的）
void Usart_SendByte(USART_TypeDef* pUSARTx,uint8_t data)//每次只能发送8位的数据
{
	//调用固件库函数
	USART_SendData(pUSARTx,data);//往串口中写入数据
	
	//发送完数据是检测TXE这个位是否置1，发送数据寄存器空了，表明已经把数据传递到数据移位寄存器了
	//检测TXE这个位也需要一个固件库函数
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);
	//如果这个位一直为0的话就一直等待，只有当被设为SET后才会跳出这个循环（表示一个字节发送出去了）

}



//有时候传感器数据可能是16位的，怎么发送？发送两个字节
//发送两个字节的数据就是十六位的
//半字表示16位，两个字节，，参数2是十六位的数据

//发送两个字节数据函数
void Usart_SendHalfWord(USART_TypeDef* pUSARTx,uint16_t data)
{
	//发送十六位数据要分为两次来发送，先定义两个变量
	uint8_t temp_h,temp_l;//定义8位的变量（分别存储高8位和低8位）

	//首先取出高8位
	temp_h=(data&0xff00)>>8;//低八位先与0相&，低8位变为0再右移8位（0xff00共16位二进制）
	//再取出低8位
	temp_l=data&0xff;//取出低8位数据
	//16位的数据这样子就放到了两个变量里面（共16位）
	
	//调用固件库函数
	USART_SendData(pUSARTx,temp_h);//先发送高8位
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//等待数据发送完毕

	USART_SendData(pUSARTx,temp_l);//再发送低8位
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE)==RESET);//等待数据发送完毕

}


//有时候数据很多想要发送数组：数组数据是8位的 
//发送8位数据的数组--需要写一个循环来调用发送一个字节的函数即可
//数组的话传进来的就是一个指针了

//发送一个数组数据
void Usart_SendArray(USART_TypeDef* pUSARTx,uint8_t *array,uint8_t num)
{
	//每次想要发送多少数据，通过形参num传进来，num定义的是8位的，那么函数最多发送255个
	int i;
	for(i=0;i<num;i++)
	{
		//调用发送一个字节函数发送数据（下面两种写法都可以）
		//Usart_SendByte(USART1,*array++);
		Usart_SendByte(USART1,array[i]);//每次只能发送8位数据
	}
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//等待发送完毕
}
//判断发送一个字节的数据标志位：USART_FLAG_TXE
//判断发送一连串字节的数据标志位：USART_FLAG_TC



//发送字符串
void Usart_SendStr(USART_TypeDef* pUSARTx,uint8_t *str)//指定串口，要发送的字符串内容
{
	uint8_t i=0;
	//使用do-while循环，do的时候已经开始发送了
	do{
		//需要调用发送一个字节函数
		Usart_SendByte(USART1,*(str+i));//发送一次之后指针地址后移一个
		i++;
	}while(*(str+i)!='\0');//最后结尾不等于'\0'为真，继续发送
	//如果='\0'表示发送完毕
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);//等待发送完毕
}



////中断服务函数（串口助手发送什么数据，就会返回什么数据）
//void USART1_IRQHandler() 
//{
//	u8 ucTemp;
//	if(USART_GetFlagStatus(USART1,USART_IT_RXNE)!=RESET)
//	{
//		ucTemp=USART_ReceiveData(USART1);
//		USART_SendData(USART1, ucTemp);
//	}
//}



////使用自定义协议接收十六进制数据

//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//		u8 Res,i;
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//		{
//			Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
//			
//			if(count==0)//如果是接收的第一个数据
//			{
//				table_data[count]=Res;//将第一个数据存到数据中第一元素
//				if(table_data[0]==0x2c)//判断接收的第一个数据是不是十六进制0X2C
//				  count++;//如果第一个数据是0X2C则表示正确计数+1
//			}
//			else if(count==1)//第一个数据接收正确的情况下，判断第二个数据
//			{
//				if(Res==0xe4)//如果刚接收的数据是0xE4则表示数据正确
//				{
//					table_data[count]=Res;//将数据储存到数组第二个元素位置
//					count++;//接收数据计数+1
//				}
//				else//如果第二个字符不是0XE4则计数清零，重新接收
//					count=0;
//			}
//			else if(count==2&&Res==0)//如果前两个数据正确，接收的第三个数据是0，则清零计数，重新接收数据
//			{
//				count=0;
//			}
//			else if(count>1&&count<9)//这是可以接收数据的范围，只要count在数据可接收数据范围内即可进行存入数据
//			{
//				table_data[count]=Res;
//				count++;
//			}
//			else if(count>=9)//如果接收数据超过数组大小，则清零重新接收
//			{
//				count=0;
//			}		
//   } 
//	 
//		memset(table_cp, 0, sizeof(table_data));//在使用数组table_cp时清空
//		for(i=0;i<9;i++)//把接收到的数据复制到table_cp数组中
//		{
//			 table_cp[i]= table_data[i];
//		
//	}
//} 



#endif	

