#include "printf_init.h"
#include "serial.h"

//����printf�����
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
* ��	�ƣ�int fputc(int ch, FILE *f)
* ��	�ܣ��ض���fputc������printf�������ն˴�ӡ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
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
* ��	�ƣ�int fgetc(FILE *f)
* ��	�ܣ��ض���fgetc������printf�������ն���������
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
int fgetc(FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}
