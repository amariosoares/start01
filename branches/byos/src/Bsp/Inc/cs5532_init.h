#ifndef _CS5532_INIT_H_
#define _CS5532_INIT_H_

#include "stm32f10x.h"
#include "includes.h"
#include "static_init.h"	   //��ʱ

//CS5532�ṹ��
typedef struct 
{
	u32 				ReferenceV;		//�ο���ѹ

	//��Ԫһ����						
	u32				SETUP_CH;				//����ͨ��
	u32				SETUP_Gain;			//����
	u32				SETUP_FRS;				//������
	u32				SETUP_UB;				//��˫��
	u32				SETUP_Latch;			//����λ
	u32				SETUP_DT;				//ת����ʱ
	u32				SETUP_OCD;				//��·���
	u32				SETUP_GE;				//����ƫ��ͨ��ѡ��
	//OS_EVENT		*ConvertEvent;			//ת������ź�
}CS5532_DCBType;
/*
	//��Ԫ������
	u32				SETUP2_CH;				//����ͨ��
	u32				SETUP2_Gain;			//����
	u32				SETUP2_FRS;				//������
	u32				SETUP2_UB;				//��˫��
	u32				SETUP2_Latch;			//����λ
	u32				SETUP2_DT;				//ת����ʱ
	u32				SETUP2_OCD;				//��·���
	u32				SETUP2_GE;				//����ƫ��ͨ��ѡ��

	OS_EVENT			*ConvertEvent;			//ת������ź�
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


void CS5532_Init(u8 chan);					//��ʼ��
u8   CS5532_Config(u8 chan,u32 sps);					//���ó�ʼ��
u32  CS5532GetValue(u8 chan);				//��ȡAD
u8   CS5532_StartConvert(u8 chan,u8 flag,u8 SetupNum);	//��ʼת��
u8   CS5532_Flag(u8 chan);					//�Ƿ�ת�����
void CS5532IsrEable(u8 chan);				//���ж����ж�
void CS5532IsrDisable(u8 chan);			//�ر��ж����ж�
CS5532_DCBType* CS5532GetDCB(u8 chan);		//��ȡ����



#endif
