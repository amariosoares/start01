#ifndef _INCLUDES_H_
#define _INCLUDES_H_


//Ԥ�Ȱ�����ͷ�ļ�************************************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"


#include "os_cpu.h"
#include "os_cfg.h"
#include "cpu.h"					//CPU�������Ͷ���

#include "app_cfg.h"
#include "init.h"		
#define reentrant
#define SYS_OPT_SEG
#define idata
#define xdata
#define _nop_();

//user add********************************************************************************************************
//�弶������ͷ�ļ�������ʵ���õ��İ弶�����û�������
//ע���˴�������ͷ�ļ�����������BspInit()����Ҫѡ����������
#include "bsp.h"

#define ARRAY_SIZE(arr)		(sizeof(arr) / sizeof((arr)[0]))

#define  DEVICE_PCF8563_EN 	  1
#define DEVICE_FM25L16_EN  	  1
#define DEVICE_MEM_STORE_EN      1
#define DEVICE_STM32_LED_EN       1
#define DEVICE_STM32_UART_EN   1
#define DEVICE_NETWORK_EN			1
#endif

