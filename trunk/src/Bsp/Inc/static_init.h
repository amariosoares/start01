#ifndef _STATIC_INIT_H_	
#define _STATIC_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f10x.h"
#include "String.h"				//memcpy
#include "math.h"				//��ѧ����
#include "includes.h"			//ucos

#define DebugPf			printf			//����printf���
//#define DebugPf

#ifndef BOOL							//BOOL
#define	BOOL			BOOLEAN
#endif

#ifndef 	TRUE							//TRUE
#define		TRUE			1
#endif

#ifndef 	FALSE 							//FALSE
#define		FALSE			0
#endif

#ifndef 	true 							//true
#define		true			1
#endif

#ifndef 	false 							//false
#define		false			0
#endif

//����״̬
#define USARTSTATUS_General		0x00	//ͨ��״̬
#define USARTSTATUS_BuffFull	0x01	//�������� 
#define USARTSTATUS_GetKeyChar	0x02	//���յ��ؼ���
#define USARTSTATUS_GetCharLen	0x04	//��������
#define USARTSTATUS_Bag			0x08	//������

typedef struct
{
	u32 ValidFlag;				//�궨����
	float  WeightVal[10];		//����ֵ
	float  VotageVal[10];		//ģ��ֵ
} CALIBRATION;					//ţ�ٲ�ֵ�궨

extern u8 test;						//�����¶ϵ���
extern CALIBRATION CalibrationSave;	//ţ�ٲ�ֵ�궨

void Delay(u32 nCount);				//ϵͳƵ�ʼ���ʱ
void Delay_US(u32 nCount);			//΢�뼶��ʱ
void Delay_MS(u32 nCount);			//���뼶��ʱ
void Delay_S(u32 nCount);			//�뼶��ʱ
void NOPDelay(void);					//NOPָ�������ӳ�
void L_From_B(float* Val,u32 size);	//ð����С����

u32 Get_SyS_CLK(u8 temp);		//��ǰϵͳ����Ƶ��״̬��ȡ	   ��ƵƵ��(0) HCLKƵ��(1) PCLK1Ƶ��(2) PCLK2Ƶ��(3) SYSTEMƵ��(����)
u16 StoB_2_BtoS(u16 n);			//12ת21
u32 StoB_4_BtoS(u32 n);			//1234ת4321 ����
float StoB_F4_BtoS(float f);		//1234ת4321 ������

u8 AsciiToHex(char ch);				//����ASCII��A-F.0-9ת16����0x0A-0x0F.0x00-0x09
char HexToAscii(u8 nHex);			//����16����0x0A-0x0F.0x00-0x09תASCII��A-F.0-9
void BufferAsciiToHex(u8 *pAaciiBuf, u8 *pHexBuf, u16 nAsciiLen);	 	//���ASCII��ת16����	 4λת2λ
void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen);			//���16����תASCII��	 2λת4λ

void GetCpuIdHex(u8* CpuId);		//��ȡCPUоƬIDֵ16���� u16 x[6];

u8 NewTon_Num(u16 flag);									//���ҵ�ǰ�궨ʹ�ø���
void NewTon_Num_init(CALIBRATION* CS,u8 num_Temp);		//ţ�ٲ��̼���
float Ca_NewTon(float ADC_temp,CALIBRATION* CS,u8 x);	//ţ�ٲ�ֵ����
long Sample_Wave(float* AD_New,float* AD_Old,long ADOld_Len,float Wave_Temp);	//����˲�
u32 ASCIItoNumber(u8* data_temp,u8 len_temp);			//ASCIIת����
u32 BeiNumber(u32 num_temp,u8 len_temp);					//���������λ�����з���
u8 JZ_chack(void);  		//8M������������ OSC_IN(5,12,23)	OSC_OUT(6,13,24)
void RCC_USEHSI(void);	//ʹ��STM32�ڲ�RCʱ�� ����ȷ ��
void System_Reset(void);	//ϵͳ����
#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
