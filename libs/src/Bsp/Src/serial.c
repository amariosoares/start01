#include "includes.h"
#include "serial.h"
#include "kfifo.h"
//#include "list.h"

#define MAX_SERIAL_NUM  		5
#define MAX_USART_BUF   		128
#define MAX_CONSOLE_BUF_SIZE 	256
typedef struct{
	struct TSerialDevice* serial_dev[MAX_SERIAL_NUM];
	u8 num;
}TSerialMgr;
static TSerialMgr serial_list;
static struct TSerialDevice* console_dev = NULL;
static struct kfifo    g_console_fifo;
static struct kfifo  *console_fifo = NULL;
static unsigned char console_buffer[MAX_CONSOLE_BUF_SIZE];
static int      console_buffer_size = MAX_CONSOLE_BUF_SIZE;
int usart_register(struct TSerialDevice* dev)
{
	if(serial_list.num >= MAX_SERIAL_NUM) return 0;

	//sprintf(dev->name,"%s%d",dev->name,dev->id);
	dev->tx_fifo = NULL;
	dev->rx_fifo = NULL;
	dev->tx_flag = 0;
	dev->open_flag = 0;
	serial_list.serial_dev[serial_list.num++] = dev;
	return 1;
}

static struct TSerialDevice* usart_get_device(const char* ttyname)
{
	for(u8 i = 0 ; i < serial_list.num; i++)
	{
		if((!strcmp(ttyname,serial_list.serial_dev[i]->name)))
			return serial_list.serial_dev[i];
	}
	return NULL;
}
struct TSerialDevice* usart_request(const char* ttyname)
{
	return usart_get_device(ttyname);
}
int usart_fifo_init(struct TSerialDevice* dev)
{
	assert_param(dev);

	if(dev){
		if(dev->mode &SERIAL_RX_INT_MODE){
			dev->rx_fifo = kfifo_alloc(MAX_USART_BUF);
			if(dev->rx_fifo == NULL)   return 0;
		}
		if(dev->mode &SERIAL_TX_INT_MODE){
			dev->tx_fifo = kfifo_alloc(MAX_USART_BUF);
			if(dev->tx_fifo == NULL)   return 0;
		}
		return 1;
	}
	return 0;
}
		
int usart_is_open(struct TSerialDevice* dev)
{
	if(dev){
		return dev->open_flag;
	}
	return 0;
}

int usart_open(struct TSerialDevice* dev,TSerialDesc* desc)
{
	assert_param(dev);
	assert_param(desc);
	assert_param(dev->open);
	if(dev){
		if(dev->open_flag) return 1;
		
		if(desc->mode == SERIAL_ERROR_MODE){
			desc->mode = SERIAL_RX_INT_MODE;
		}
		dev->mode = desc->mode;
		if(!usart_fifo_init(dev)) return 0;
		
		if(dev->open(dev,desc)){
			dev->open_flag = 1;
			return 1;
		}
	}
	return 0;
}

u8	 usart_portnum(void)
{
	return  serial_list.num;
}
int usart_close(struct TSerialDevice* dev)
{
	assert_param(dev && dev->close);
	
	if(dev && dev->open_flag){
			return dev->close(dev);
	}
	return 0;

}

u32  usart_sendstring(struct TSerialDevice* dev, u8* data, u32 len)
{
	assert_param(dev);
	assert_param(dev->sendstring != NULL);
	assert_param(dev->open_flag == 1);
	if(dev && dev->open_flag){
		return dev->sendstring(dev, data, len);
	}
	return 0;

}

int  usart_sendbyte(struct TSerialDevice* dev,u8 ch)
{
	
	assert_param(dev && dev->sendbyte);
	assert_param(dev->open_flag);
	if(dev ){
		return dev->sendbyte(dev, ch);
	}
	return 0;

}

u32   usart_getpacket(struct TSerialDevice* dev, u8* data, u32 len)
{
	
	assert_param(dev && dev->getpacket);
	assert_param(dev->open_flag);
	if(dev){
		return dev->getpacket(dev, data, len);
	}
	return 0;

}

u32   usart_getdatasize(struct TSerialDevice* dev)
{
	
	assert_param(dev && dev->getdatasize);
	assert_param(dev->open_flag);
	if(dev){
		return dev->getdatasize(dev);
	}
	return 0;

}
int register_console(struct TSerialDevice* dev)
{
	console_dev = dev;

	if(console_fifo && console_dev){
		u8 ch;
		while(0==kfifo_getc(console_fifo,&ch)){
			console_putc(ch);
		}
	}
	return 1;
}
void   console_alloc_kfifo(void)
{
	if(console_fifo==NULL){
		console_fifo =&g_console_fifo;
		kfifo_init_static(console_fifo,console_buffer,console_buffer_size);
	}
}
void  console_putc(char ch)
{
	if(console_dev){
		usart_sendbyte(console_dev,ch);
	}else{
		if(console_fifo==NULL)
			console_alloc_kfifo();
		if(console_fifo){
			kfifo_putc(console_fifo,ch);
		}
	}
}
void console_putstring(const char* text)
{
	while(*text){
		console_putc(*text++);
	}
}
static int serial_init(void)
{
	console_alloc_kfifo();

	return 1;
}
coredevice_initcall(serial_init);

