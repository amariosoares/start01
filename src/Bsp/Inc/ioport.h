#ifndef	_IOPORT_H_
#define _IOPORT_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

//private=====================================================================

#define PORT(x)		GPIO##x				   //IO端口
#define BIT(x)		(1<<x)				   //IO引脚

//public======================================================================
/////////////////////////////IO端口属性/////////////////////////////////
#define GPIO_ANALOG_INPUT			0
#define GPIO_FLOATING_INPUT			4
#define GPIO_PULL_UP_DOWN_INPUT		8
#define GPIO_PUSH_PULL_10M_OUTPUT	1
#define GPIO_PUSH_PULL_2M_OUTPUT	2
#define GPIO_PUSH_PULL_50M_OUTPUT	3
#define GPIO_OPEN_DRAIN_10M_OUTPUT	5
#define GPIO_OPEN_DRAIN_2M_OUTPUT	6
#define GPIO_OPEN_DRAIN_50M_OUTPUT	7
#define AFIO_PUSH_PULL_10M_OUTPUT	9
#define AFIO_PUSH_PULL_2M_OUTPUT	10
#define AFIO_PUSH_PULL_50M_OUTPUT	11
#define AFIO_OPEN_DRAIN_10M_OUTPUT	13
#define AFIO_OPEN_DRAIN_2M_OUTPUT	14
#define AFIO_OPEN_DRAIN_50M_OUTPUT	15

///////////////////////////设置IO端口属性/////////////////////////////////
#define SET_IO_ATTRI(port, bit, attri)  	(*(&(port->CRL) + ((bit)>>3))) &= ~(0x0FUL << 4*(bit&0x07)); \
											(*(&(port->CRL) + ((bit)>>3))) |= (attri << 4*(bit&0x07))	 //设置IO属性


#define IoPushPull(bit)				SET_IO_ATTRI(bit##_PORT, bit, GPIO_PUSH_PULL_50M_OUTPUT)	 //配置管脚推挽输出
#define IoOpenDrain(bit)			SET_IO_ATTRI(bit##_PORT, bit, GPIO_OPEN_DRAIN_50M_OUTPUT)	 //配置管脚开漏输出
#define IoInFloating(bit)			SET_IO_ATTRI(bit##_PORT, bit, GPIO_FLOATING_INPUT)			 //配置管教浮空输入

#define IoHi(bit)					(bit##_PORT##->ODR) |= 1<<bit 					   //引脚置1
#define IoLo(bit)					(bit##_PORT##->ODR) &= ~(1<<bit)				   //引脚置0

#define GetIo(bit)				((bit##_PORT##->IDR & (1<<bit)) >> bit)	   			//读取引脚电平

#define DataInitZero(x) 			memset(x, 0, sizeof(x))			//将缓冲区x的数据空间初始化为0
#define DataInit(x, nData)			memset(x, nData, sizeof(x))		//将缓冲区x的数据赋值为nData
		
//************************************************************************
#endif

#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

/*
#define X 1						//X必须一致 1引脚号
#define X_PORT	GPIOB		   	//端口后面加_PORT

MakePushPull(X);				//将引脚设置成为推拉输出
MakeOpenDrain(X);				//将引脚设置成为开漏输出
SetHi(X);						//置1
SetLo(X);						//置0
bool GetSignal(X);				//读取引脚电平
*/
