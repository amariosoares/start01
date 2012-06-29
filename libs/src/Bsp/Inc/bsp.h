#ifndef  _BSP_H_
#define  _BSP_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "includes.h"	   //�ϰ汾�����Ͷ��弰ϵͳ��Դ

//�ײ�
#include "nvic_init.h"	   		//�жϹ���
#include "printf_init.h"   		//��ӡ���
#include "tim2_init.h"	   		//TIM2	����1-4������ʱ����
#include "cs5532_init.h"   		//CS5532
#include "at45db321d_init.h"	//DataFlash

//Ӧ�ò�


#define RCC_PLLMUL RCC_PLLMul_9 
void SysTickConfig(void);
u8 get_APB1_prevdiv(void);
void BSP_Init(void);		  	//Ӳ����ʼ��

#ifdef __cplusplus		   		//�����CPP����C���� //��������
}
#endif

#endif      /* End of module include. */
