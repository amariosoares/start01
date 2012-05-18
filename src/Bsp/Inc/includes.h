#ifndef _INCLUDES_H_
#define _INCLUDES_H_


//预先包含的头文件************************************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"


#include "os_cpu.h"
#include "os_cfg.h"
#include "cpu.h"					//CPU数据类型定义

#include "app_cfg.h"
#include "init.h"		
#define reentrant
#define SYS_OPT_SEG
#define idata
#define xdata
#define _nop_();

//user add********************************************************************************************************
//板级开发包头文件，根据实际用到的板级驱动用户自增减
//注：此处包含的头文件将决定函数BspInit()中所要选择编译的内容
#include "bsp.h"

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

#define  DEVICE_PCF8563_EN 	  1
#define DEVICE_FM25L16_EN  	  1
#define DEVICE_MEM_STORE_EN      1
#define DEVICE_STM32_LED_EN       1
#define DEVICE_STM32_UART_EN   1
#define DEVICE_NETWORK_EN			1
#endif

