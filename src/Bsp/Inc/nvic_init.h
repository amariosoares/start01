#ifndef _NVIC_INIT_H_
#define _NVIC_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif
 
#include "stm32f10x.h"

void nvic_init(void);	   //�жϹ����ʼ��

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
