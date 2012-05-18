#include "printf_init.h"
#include "serial.h"

//定义printf输出口
#define USART_PRINTF	USART1
//#define USART_PRINTF	USART2
//#define USART_PRINTF	USART3
//#define USART_PRINTF	UART4
//#define USART_PRINTF	UART5

#pragma import(__use_no_semihosting)
                           
struct __FILE 
{ 
	int handle; 
};
 
FILE __stdout;
         
int _sys_exit(int x) 
{ 
	x = x; 
	return 0;
}

/****************************************************************************
* 名	称：int fputc(int ch, FILE *f)
* 功	能：重定义fputc函数，printf函数从终端打印输出
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
int fputc(int ch, FILE *f)
{   
#if 1
	if(ch == '\n')
		console_putc('\r');
	console_putc(ch);
#else
	USART_SendData(UART4, ch);
	 while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
#endif
	return ch;
}
/****************************************************************************
* 名	称：int fgetc(FILE *f)
* 功	能：重定义fgetc函数，printf函数从终端输入数据
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
