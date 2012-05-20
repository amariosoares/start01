#include "serial.h"
#include "kfifo.h"
#include "clock.h"
#include "timer.h"

void debug_ll_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;	//���ڷ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//Ƶ��50MHz
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ������

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;	//���ڷ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ������
						
}


#if (DEVICE_STM32_UART_EN > 0)

#define MAX_USART_NUM 	5
typedef enum{
	INDEX_UART1=0,
	INDEX_UART2,
	INDEX_UART3,
	INDEX_UART4,
	INDEX_MAX,
}SERIAL_INDEX;
static BOOL request_irq(int irq);
static TSerialDesc defdesc = {
	.baudrate  = B115200,
	.databit   = DataBit8,
	.stopbit   = StopBit1,
	.parity    = PARITY_NONE,
	.flowctrl = FLOWCTRL_NONE,
	.mode     = SERIAL_RX_INT_MODE,
};
typedef struct{
	USART_TypeDef* usart;
	u8	uart_id;
	GPIO_TypeDef*  gpio[2];
	DMA_Channel_TypeDef* dma;	
	u16	pin_rx;
	u16   pin_tx;
	
	u8  pin_remap;
	u8  dma_tx_buff[128];
	u8  packet_mode;
	u8    rx_tx_ch;
	u32	dma_base;
	int 	irq;
	int 	dma_irq;
	u32 	remap_base;
	volatile u32	packet_complete;
	u32 	timeout_us;
	uint64_t start;
	struct TSerialDevice  dev;
	//struct TimerDevice* timer;
	//TimerDeviceCfg*	 timer_cfg;
	
}STM32_USART_DESC;



//static struct TSerialDevice serdev[MAX_USART_NUM];
static u32 stm32_uart_getstring(struct TSerialDevice* dev, u8* data, u32 len, s32 timeout_ms);
static STM32_USART_DESC stm32_usart[] = {
		[INDEX_UART1]={
			.usart  	= USART1,
			.uart_id       = 1,
			.gpio 		= {GPIOA,GPIOA},
			.dma    	= DMA1_Channel4,
			.pin_rx 	= GPIO_Pin_10,
			.pin_tx 	= GPIO_Pin_9,
			.pin_remap  = 0,
			.irq		= USART1_IRQn,
			.dma_irq	= DMA1_Channel4_IRQn,
			.dma_base   = USART1_BASE + 4,
			.timeout_us = ((1000000/(115200/3)) * 10),	//���ճ�ʱ��	��/������/10*3����/20us TIM2�ļ���Ƶ��
		},
		[INDEX_UART2]={
			.usart  	= USART2,
			.uart_id       = 2,
			.gpio 		= {GPIOD,GPIOD},
			.dma    	= DMA1_Channel4,
			.pin_rx 	= GPIO_Pin_6,
			.pin_tx 	= GPIO_Pin_5,	
#if (defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_CL))
			.remap_base = GPIO_Remap_USART2,
			.pin_remap  = 1,
#else
			.pin_remap  = 0,
#endif
			.irq		= USART2_IRQn,
			.dma_irq	= DMA1_Channel4_IRQn,
			.dma_base   = USART2_BASE + 4,
			.timeout_us = ((1000000/(115200/3)) * 10),	//���ճ�ʱ��	��/������/10*3����/20us TIM2�ļ���Ƶ��
		},
		[INDEX_UART3]={
			.usart  	= USART3,
			.uart_id       = 3,
			.gpio 	= {GPIOB,GPIOB},
			.dma    	= DMA1_Channel4,
			.pin_rx 	= GPIO_Pin_11,
			.pin_tx 	= GPIO_Pin_10,	
			.pin_remap  = 0,
			.irq		= USART3_IRQn,
			.dma_irq	= DMA1_Channel4_IRQn,
			.dma_base   = USART3_BASE + 4,
			.timeout_us = ((1000000/(115200/3)) * 10),	//���ճ�ʱ��	��/������/10*3����/20us TIM2�ļ���Ƶ��
		},
#if 1
		[INDEX_UART4]={
			.usart  	= UART4,
			.uart_id       = 4,
			.gpio 	= {GPIOC,GPIOC},
			.dma    	= DMA1_Channel4,
			.pin_rx 	= GPIO_Pin_11,
			.pin_tx 	= GPIO_Pin_10,	
			.pin_remap  = 0,
			.irq		= UART4_IRQn,
			.dma_irq	= DMA1_Channel4_IRQn,
			.dma_base   = UART4_BASE + 4,
			.timeout_us = ((1000000/(115200/3)) * 10),	//���ճ�ʱ��	��/������/10*3����/20us TIM2�ļ���Ƶ��
		},
#endif

};
void stm32_dma_init(STM32_USART_DESC* desc)
{
	DMA_InitTypeDef  DMA_InitStructure;			//DMA�ṹ

	if(!desc->dev.mode&SERIAL_DMA_MODE) return;
				   
	
	request_irq(desc->dma_irq);

	//DMA����
	DMA_DeInit(desc->dma);  		   									//��λDMA1_Channel4ͨ��ΪĬ��ֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = desc->dma_base;				//DMAͨ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr 	 = (u32)desc->dma_tx_buff;				//DMAͨ���洢������ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//DMAĿ�ĵ�	(DMA_DIR_PeripheralSRCԴ������)  ˫����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//��ǰ����Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//��ǰ�洢�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//�������ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//�洢�����ݿ��Ϊ�ֽ�(8λ)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							//��������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					//DMAͨ�����ȼ��ǳ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMAͨ��δ���ô洢�����洢������
	DMA_Init(desc->dma, &DMA_InitStructure);							//�����������ó�ʼ��DMA
	DMA_ITConfig(desc->dma, DMA_IT_TC, ENABLE);    						//����DMAͨ���ж�
}
static BOOL stm32_usart_gpio_init(STM32_USART_DESC* desc)			//�������ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;		//�������Žṹ

	if(desc==NULL) return FALSE;

	if(desc->pin_remap){
    		GPIO_PinRemapConfig(desc->remap_base, ENABLE);
	}
	GPIO_InitStructure.GPIO_Pin   = desc->pin_tx;	//���ڷ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//Ƶ��50MHz
	
	GPIO_Init(desc->gpio[0], &GPIO_InitStructure);//��ʼ������

	GPIO_InitStructure.GPIO_Pin   = desc->pin_rx;	//���ڷ�������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	
	
	GPIO_Init(desc->gpio[1], &GPIO_InitStructure);//��ʼ������
		
	return TRUE;
}


void stm32_param_config(STM32_USART_DESC* uart,TSerialDesc* desc)
{		
	USART_InitTypeDef UART_InitStructure;		//���ڽṹ

	switch(desc->baudrate)
	{
		case B115200:
			UART_InitStructure.USART_BaudRate = 115200;
			break;	
		case B38400:
			UART_InitStructure.USART_BaudRate = 38400;
			break;
		case B9600:
			UART_InitStructure.USART_BaudRate = 9600;
			break;
		default:
			UART_InitStructure.USART_BaudRate = 115200;
			break;
	}
	//databit
	switch(desc->databit)
	{
		case DataBit8:
			UART_InitStructure.USART_WordLength = USART_WordLength_8b;
			break;
		case DataBit9:
			UART_InitStructure.USART_WordLength = USART_WordLength_9b;
			break;
		default:
			UART_InitStructure.USART_WordLength = USART_WordLength_8b;
			break;
	}
	//stopbit
	switch(desc->stopbit)
	{
		case StopBit1:
			UART_InitStructure.USART_StopBits = USART_StopBits_1;
			break;
		case StopBit2:
			UART_InitStructure.USART_StopBits = USART_StopBits_2;
			break;
		default:
			UART_InitStructure.USART_StopBits = USART_StopBits_1;
			break;
	}

	switch(desc->parity)
	{
		case PARITY_NONE:
			UART_InitStructure.USART_Parity = USART_Parity_No;
			break;
		case PARITY_ODD:
			UART_InitStructure.USART_Parity = USART_Parity_Odd;
			break;
		case PARITY_EVEN:
			UART_InitStructure.USART_Parity = USART_Parity_Even;
			break;
		default:
			UART_InitStructure.USART_Parity = USART_Parity_No;
			break;
	}
	switch(desc->flowctrl)
	{
		case FLOWCTRL_NONE:
			UART_InitStructure.USART_Parity = USART_HardwareFlowControl_None;
			break;
		default:
			UART_InitStructure.USART_Parity = USART_HardwareFlowControl_None;
			break;
	}
	UART_InitStructure.USART_Mode   = USART_Mode_Rx | USART_Mode_Tx;	//���ͺͽ���ʹ��
	USART_Init(uart->usart, &UART_InitStructure);	//��ʼ������
}


static BOOL request_irq(int irq)
{
	NVIC_InitTypeDef NVIC_InitStructure; 		//�жϿ���������

	NVIC_InitStructure.NVIC_IRQChannel = irq;			//�����ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//�򿪴����ж�
	NVIC_Init(&NVIC_InitStructure);								//��ʼ���ж�������

	return TRUE;
}


static void stm32_open_uart_interrput(STM32_USART_DESC* desc,BOOL enable)
{

	USART_ClearITPendingBit(desc->usart, USART_IT_RXNE);				//����ձ�־
	if(desc->dev.mode&SERIAL_RX_INT_MODE)
		USART_ITConfig(desc->usart, USART_IT_RXNE, ENABLE);				//���������ж�
	
	USART_ClearITPendingBit(desc->usart, USART_IT_TC);				//����ձ�־
	USART_ITConfig(desc->usart, USART_IT_TC, DISABLE);				//���������ж�

	USART_Cmd(desc->usart, enable);  								//ʹ��ʧ�ܴ�������

}


static BOOL stm32_uart_enable_clock( STM32_USART_DESC* uart,BOOL enable)
{
	BOOL ret = FALSE;
	if(uart){
		FunctionalState state = enable?ENABLE:DISABLE;
		switch(uart->uart_id){
			case 1:
				{
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, state);
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,  state);
					if(uart->dev.mode&SERIAL_DMA_MODE){
						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
					}
					ret = TRUE;
					break;
				}
			case 2:
				{
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, state);	
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO,  state);
					if(uart->dev.mode&SERIAL_DMA_MODE){
						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
					}
					ret = TRUE;
					break;
				}
			case 3:
				{
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, state);
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,  state);
					if(uart->dev.mode&SERIAL_DMA_MODE){
						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
					}
					ret = TRUE;
					break;
				}
			case 4:
				{
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, state);	
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, state);
					if(uart->dev.mode&SERIAL_DMA_MODE){
						RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);			//ʹ��DMA2ʱ��
					}
					
					ret = TRUE;
					break;
				}
			case 5:
				{
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, state);	
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, state);				
					ret = TRUE;
					break;
				}
			default:
				break;
				
		}
	}

	return ret;
	
}

static void usart_packet_timeout_callback(TimerID id,void* arg)
{
	STM32_USART_DESC* uart = (STM32_USART_DESC*)arg;
	if(uart){
		uart->packet_complete  = 1;
		uart->packet_mode  	   = 0;
	}
}
void usart_irq_route(STM32_USART_DESC* stm32uart)
{
	
	//�����жϴ���
	if(USART_GetITStatus(stm32uart->usart, USART_IT_RXNE) == SET)		
	{
		//���ݽ��ն���
		USART_ClearITPendingBit(stm32uart->usart, USART_IT_RXNE);				//��ս����жϱ�־
		stm32uart->start = get_time_ns();
		kfifo_putc(stm32uart->dev.rx_fifo,USART_ReceiveData(stm32uart->usart));
	}
	//�Ĵ�����һ���ֽڵ����ݷ������
	if(USART_GetITStatus(stm32uart->usart, USART_IT_TC) == SET )
	{
		USART_ClearITPendingBit(stm32uart->usart, USART_IT_TC);				//��ս����жϱ�־
		if(stm32uart->dev.tx_fifo){
			u8 ch;
			if(-1 == kfifo_getc(stm32uart->dev.tx_fifo,&ch)){	
				USART_ITConfig(stm32uart->usart, USART_IT_TC, DISABLE);
			}else{
				USART_SendData(stm32uart->usart,ch);
			}
		}
	}

}
void USART1_IRQHandler(void)
{
	usart_irq_route(&stm32_usart[INDEX_UART1]);
}
			  
void USART2_IRQHandler(void)
{
	usart_irq_route(&stm32_usart[INDEX_UART2]);	
}
void USART3_IRQHandler(void)
{
	usart_irq_route(&stm32_usart[INDEX_UART3]);	
}

void UART4_IRQHandler(void)
{
	usart_irq_route(&stm32_usart[INDEX_UART4]);
}

#if 0



void UART5_IRQHandler(void)
{
	usart_irq_route(&stm32_usart[4]);	
}



/****************************************************************************
* ��	�ƣ�void DMA1_Channel4_IRQHandler(void)	
* ��	�ܣ�DMA�жϻ���
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
	OSIntEnter();	//�����жϽ����������ȼ����ȱ��� 
		  
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)) //����������
	{
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);    //�ر�DMA����
		DMA_Cmd(DMA1_Channel4, DISABLE);	       			//�ر�DMAͨ��  
	}

	DMA_ClearFlag(DMA1_FLAG_GL4| DMA1_FLAG_TC4 | DMA1_FLAG_HT4 | DMA1_FLAG_TE4);  //���DMA��ر�־
		
	OSIntExit();   //�˳��жϽ����������ȼ����µ���
}
#endif


static u32	stm32_uart_sendstring(struct TSerialDevice* dev, u8* data, u32 len)
{
	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;

	if((data==NULL) || (len ==0)) return 0;
	
	if(dev->mode&SERIAL_TX_INT_MODE){
		if(dev->tx_fifo==NULL) return 0;
		while(kfifo_len(dev->tx_fifo) > 0);
		
		kfifo_put(dev->tx_fifo,data,len);
		if(0==kfifo_getc(dev->tx_fifo,&(uart->rx_tx_ch)))
		{
			uart->usart->DR = uart->rx_tx_ch;
			USART_ITConfig(uart->usart, USART_IT_TC, ENABLE);				//���������ж�
		}
	}else{
		for(u32 i = 0 ; i < len; i++)
		{
			USART_SendData(uart->usart, data[i]);
			while(USART_GetFlagStatus(uart->usart, USART_FLAG_TXE) == RESET);
		}
	}
	
	return len;

}
static u32  stm32_uart_sendbyte(struct TSerialDevice* dev, u8 ch)
{
	
	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;
	USART_SendData(uart->usart, ch);
   	 while(USART_GetFlagStatus(uart->usart, USART_FLAG_TXE) == RESET);

	return 1;
}
static BOOL stm32_uart_close(struct TSerialDevice* dev)
{
	return TRUE;
}
static BOOL stm32_uart_open(struct TSerialDevice* dev,TSerialDesc* desc)
{

	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;

	if(uart==NULL) return FALSE;

	

	if(!stm32_uart_enable_clock(uart,TRUE)) return FALSE;

	if(!stm32_usart_gpio_init(uart)) return FALSE;

	stm32_param_config(uart,desc);
	
	if(!request_irq(uart->irq)) return FALSE;

	//stm32_dma_init(uart);
	
	stm32_open_uart_interrput(uart,TRUE);
	
	return TRUE;
	
}
static u32 stm32_uart_getdatasize(struct TSerialDevice* dev)
{
	return kfifo_len(dev->rx_fifo);
}

BOOL  stm32_uart_set_timeout(struct TSerialDevice* dev,u32 timeout_us)
{
	
	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;
	if(uart){
		uart->timeout_us = timeout_us;
	}
	return FALSE;
}
static u32 stm32_uart_getpacket(struct TSerialDevice* dev, u8* data, u32 len)
{
#if 0	
	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;

	return 0;
	uart->packet_complete = 0;
	uart->packet_mode = 1;
	if(uart->uart_event){
		
		u8 BoxErr=0;	
		if(uart->timer == NULL){
			goto err;
		}
		OSMboxPend(uart->uart_event,0,&BoxErr);
		uart->packet_mode = 0;
		if(BoxErr == 0) 										//������ճɹ�
		{
			len = (len < kfifo_len(dev->fifo))?len:kfifo_len(dev->fifo);
			return kfifo_get(dev->fifo,data,len);//�����ݻ�ȡ����
		}
		
	}else{
		while(!uart->packet_complete){
				
		}
	}
	
err:	
	return kfifo_get(dev->fifo,data,len);	
#else
	return stm32_uart_getstring(dev,data,len,0);

	#endif
	
	//return 0;
}
static u32 stm32_uart_getstring(struct TSerialDevice* dev, u8* data, u32 len, s32 timeout_ms)
{
	
	STM32_USART_DESC* uart = (STM32_USART_DESC*)dev->private_data;
	uart->start = get_time_ns();
	while(1){
		if(timeout_ms >= 0 )
		{
			if(is_timeout(uart->start, timeout_ms*MSECOND) || 
			(kfifo_len(dev->rx_fifo) >= len))
				break;
		}
		else if ((kfifo_len(dev->rx_fifo) >= len)) {
			break;
		}
		mdelay(1);
	}
	return kfifo_get(dev->rx_fifo,data,len);	
}

#define DRV_NAME "ttystm"

static int stm32_serial_init(void)
{
	
	for(u8 i =0 ; i < ARRAY_SIZE(stm32_usart); i++){

		stm32_usart[i].dev.open  		= stm32_uart_open;
		stm32_usart[i].dev.close 			= stm32_uart_close;
		stm32_usart[i].dev.sendbyte		= stm32_uart_sendbyte;
		stm32_usart[i].dev.getdatasize 	= stm32_uart_getdatasize;
		stm32_usart[i].dev.getstring 		= stm32_uart_getstring;
		stm32_usart[i].dev.sendstring 	= stm32_uart_sendstring;
		stm32_usart[i].dev.getpacket		= stm32_uart_getpacket;
	
		stm32_usart[i].dev.private_data	= &stm32_usart[i];
		stm32_usart[i].dev.id			= i+1;
		memset(stm32_usart[i].dev.name,MAX_SERIAL_NAME,0);
	   	 snprintf(stm32_usart[i].dev.name,MAX_SERIAL_NAME,"%s%d",DRV_NAME, stm32_usart[i].dev.id);
		if(!usart_register(&(stm32_usart[i].dev)))
			return 0;
	}
	
	return 1;
}
device_initcall(stm32_serial_init);
#endif
