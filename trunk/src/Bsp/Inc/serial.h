#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus			//定义对CPP进行C处理
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"	//串口结构体
#define SERIAL_ERROR_MODE 	0x0
#define SERIAL_RX_INT_MODE 	0x1
#define SERIAL_TX_INT_MODE 	0x2
#define SREIAL_POLL_MODE	  	0x4
#define SERIAL_DMA_MODE		 0x8
typedef enum{
	B4800=0,
	B9600,
	B19200,
	B38400,
	B57600,
	B115200
}TSerailBaudRate;

typedef enum{
	DataBit8=0,
	DataBit9,
}TSerailDataBit;

typedef enum{
	StopBit1=0,
	StopBit2,
}TSerailStopBit;

typedef enum{
	PARITY_NONE=0,
	PARITY_ODD,
	PARITY_EVEN,
	PARITY_MARK, 
	PARITY_SPACE,
}TSerailPariry;

typedef enum{
	FLOWCTRL_NONE=0,
	FLOWCTRL_SOFTWARE,
	FLOWCTRL_HARDWARE,
}TSerailFlowCtrl;

typedef struct{ 
		TSerailBaudRate 		baudrate;				//波特率
		TSerailDataBit			databit;
		TSerailStopBit			stopbit;
		TSerailPariry			parity;
		TSerailFlowCtrl		flowctrl;
		int					mode;
}TSerialDesc;

#define MAX_SERIAL_NAME 16
struct TSerialDevice{
	char name[MAX_SERIAL_NAME];
	u8   id; //串口序号		
	int (*open)(struct TSerialDevice* dev,TSerialDesc* desc);
	int (*close)(struct TSerialDevice* dev);
	int (*reset)(struct TSerialDevice* dev);
	u32  (*sendbyte)(struct TSerialDevice* dev,u8 ch);
	u32  (*sendstring)(struct TSerialDevice* dev,u8* data, u32 len);
	u32  (*dma_sendstring)(struct TSerialDevice* dev,u8* data, u32 len);
	int  (*getbyte)(struct TSerialDevice* dev,u8* ch);
	u32  (*getstring)(struct TSerialDevice* dev,u8* data, u32 len,s32 timeout_ms);	
	u32  (*getpacket)(struct TSerialDevice* dev, u8* data, u32 len);
	int  (*setpacket_timeout)(struct TSerialDevice* dev,u32 timeout_us);
	u32  (*getdatasize)(struct TSerialDevice* dev);
	int (*enable_clk)(struct TSerialDevice* dev,u8 enable);
	struct kfifo* rx_fifo;
	struct kfifo* tx_fifo;
	volatile int    tx_flag;
	int   mode;
	int   open_flag;
	void*  private_data;
	TSerialDesc* desc;
	u16    rx_char;
};
typedef struct {
	char name[16];
	int  num;
}TUartPlatForm;
struct TSerialDevice* usart_request(const char* ttyname);
int usart_open(struct TSerialDevice* dev,TSerialDesc* desc);
int usart_is_open(struct TSerialDevice* dev);
u8	 usart_portnum(void);
int usart_close(struct TSerialDevice* dev);
u32  usart_sendstring(struct TSerialDevice* dev, u8* data, u32 len);
int  usart_sendbyte(struct TSerialDevice* dev,u8 ch);
u32   usart_getpacket(struct TSerialDevice* dev, u8* data, u32 len);
u32   usart_getdatasize(struct TSerialDevice* dev);
void  console_putc(char ch);
void console_putstring(const char* text);
int register_console(struct TSerialDevice* dev);

/*
timeout: 
	0: immetery return avalible data
	>0 : wait timeout util data length equal len
	-1: wait util data length equal len
*/
u32   usart_getdata(struct TSerialDevice* dev, u8* data, u32 len,s32 timeout_ms);
/*
timeout:
	0: immetery return avalible data
	1: wait data util timeout from start reqeust data
*/
u32   usart_getpacket(struct TSerialDevice* dev, u8* data, u32 len);

int  usart_set_timeout(struct TSerialDevice* dev,u32 timeout_us);
int  usart_register(struct TSerialDevice* dev);
#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
