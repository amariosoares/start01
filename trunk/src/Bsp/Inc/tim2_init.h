#ifndef _TIM2_INIT_H_	
#define _TIM2_INIT_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"  //串口结构体	系统频率获取  中断优先级保护

void TIM2_Config(void);	   		//TIM2配置

#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
