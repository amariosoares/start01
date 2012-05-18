#ifndef  _BSP_H_
#define  _BSP_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

#include "includes.h"	   //老版本的类型定义及系统资源

//底层
#include "nvic_init.h"	   		//中断管理
#include "printf_init.h"   		//打印输出
#include "tim2_init.h"	   		//TIM2	串口1-4接收延时控制
#include "cs5532_init.h"   		//CS5532
#include "at45db321d_init.h"	//DataFlash

//应用层


#define RCC_PLLMUL RCC_PLLMul_9 
void SysTickConfig(void);
u8 get_APB1_prevdiv(void);
void BSP_Init(void);		  	//硬件初始化

#ifdef __cplusplus		   		//定义对CPP进行C处理 //结束部分
}
#endif

#endif      /* End of module include. */
