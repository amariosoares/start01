#ifndef _TIM2_INIT_H_	
#define _TIM2_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"  //���ڽṹ��	ϵͳƵ�ʻ�ȡ  �ж����ȼ�����

void TIM2_Config(void);	   		//TIM2����

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
