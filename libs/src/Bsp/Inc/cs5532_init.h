#ifndef _CS5532_INIT_H_
#define _CS5532_INIT_H_

#include "stm32f10x.h"
#include "includes.h"
#include "static_init.h"	   //延时

//CS5532结构体
typedef struct 
{
	u32 				ReferenceV;		//参考电压

	//单元一配置						
	u32				SETUP_CH;				//物理通道
	u32				SETUP_Gain;			//增益
	u32				SETUP_FRS;				//字速率
	u32				SETUP_UB;				//单双极
	u32				SETUP_Latch;			//锁存位
	u32				SETUP_DT;				//转换延时
	u32				SETUP_OCD;				//开路检测
	u32				SETUP_GE;				//增益偏移通道选择
	//OS_EVENT		*ConvertEvent;			//转换完成信号
}CS5532_DCBType;
/*
	//单元二配置
	u32				SETUP2_CH;				//物理通道
	u32				SETUP2_Gain;			//增益
	u32				SETUP2_FRS;				//字速率
	u32				SETUP2_UB;				//单双极
	u32				SETUP2_Latch;			//锁存位
	u32				SETUP2_DT;				//转换延时
	u32				SETUP2_OCD;				//开路检测
	u32				SETUP2_GE;				//增益偏移通道选择

	OS_EVENT			*ConvertEvent;			//转换完成信号
*/

typedef struct {
	GPIO_TypeDef* group;
	u16     	  index;
}PIN_TypeDef;

typedef struct{
	u8  port_src;
	u8  pin_src;
	u8  irq_chan;
	u32 exti_line;
}EXTI_Typedef;
typedef struct {
	PIN_TypeDef MISO_Pin;
	PIN_TypeDef MOSI_Pin;
	PIN_TypeDef SCLK_Pin;
	PIN_TypeDef CS_Pin;	
	EXTI_Typedef Exti_cfg;
}CS5532_TypeDef;


void CS5532_Init(u8 chan);					//初始化
u8   CS5532_Config(u8 chan,u32 sps);					//配置初始化
u32  CS5532GetValue(u8 chan);				//读取AD
u8   CS5532_StartConvert(u8 chan,u8 flag,u8 SetupNum);	//开始转换
u8   CS5532_Flag(u8 chan);					//是否转换完成
void CS5532IsrEable(u8 chan);				//打开中断线中断
void CS5532IsrDisable(u8 chan);			//关闭中断线中断
CS5532_DCBType* CS5532GetDCB(u8 chan);		//获取配置



#endif
