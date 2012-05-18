#include "static_init.h"

u8 test=0;					//�����¶ϵ���
/****************************************************************************
* ��	�ƣ�void Delay(u32 nCount)
* ��	�ܣ�һ��������Ƶ����ʱ
* ��ڲ�����u32 nCount	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay(u32 nCount)
{
  while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_US(u32 nCount_temp)
* ��	�ܣ�΢�뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_US(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_MS(u32 nCount_temp)
* ��	�ܣ����뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_MS(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�void Delay_S(u32 nCount_temp)
* ��	�ܣ��뼶
* ��ڲ�����u32 nCount_temp	��ʱ��
* ���ڲ�������
* ˵	������
****************************************************************************/
void Delay_S(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000000;
	while(nCount--);
}

/****************************************************************************
* ��	�ƣ�__asm void NOPDelay(void)
* ��	�ܣ�NOPָ�������ӳ�	   (���һ���ӳ�1΢��)
* ��ڲ�������
* ���ڲ�������
* ˵	��������һ����ʱһ��ϵͳʱ������	  
****************************************************************************/
__asm void NOPDelay(void)	  //NOPָ�����ڼ��ӳ�
{
	NOP;
}

/****************************************************************************
* ��	�ƣ�u32 Get_SyS_CLK(u8 temp)
* ��	�ܣ���ǰϵͳ����Ƶ��״̬��ȡ	   
* ��ڲ�����temp ѡ��Ƶ�ʲ���
* ���ڲ�����ʱ��Ƶ��
* ˵	������ƵƵ��(0) HCLKƵ��(1) PCLK1Ƶ��(2) PCLK2Ƶ��(3) SYSTEMƵ��(����)
			x=Get_SyS_CLK(1); //��ƵƵ��
			printf("ADCCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(2); //HCLKƵ��
			printf("HCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(3); //PCLK1Ƶ��
			printf("PCLK1=%ldMHz\r\n",x);
			x=Get_SyS_CLK(4); //PCLK2Ƶ��
			printf("PCLK2=%ldMHz\r\n",x);
			x=Get_SyS_CLK(5); //SYSTEMƵ�� //����
			printf("SYSCLK=%ldMHz\r\n",x);			
****************************************************************************/
u32 Get_SyS_CLK(u8 temp)
{
	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);	//Ϊʱ�ӱ�������Ƭ�ϸ�ʱ��
	switch (temp)
	{
		case 1:	return RCC_ClockFreq.ADCCLK_Frequency; 	//��ƵƵ��
		case 2:	return RCC_ClockFreq.HCLK_Frequency; 	//HCLKƵ��
		case 3:	return RCC_ClockFreq.PCLK1_Frequency; 	//PCLK1Ƶ��
		case 4:	return RCC_ClockFreq.PCLK2_Frequency; 	//PCLK2Ƶ��
		default: return RCC_ClockFreq.SYSCLK_Frequency;	//SYSTEMƵ��
	}	
}


/****************************************************************************
* ��	�ƣ�u8 AsciiToHex(char ch)
* ��	�ܣ�����ASCII��ת16����		   
* ��ڲ�����ch	'A'-'F'��'0'-'9'
* ���ڲ�����ת�����0x0A-0x0F.0x00-0x09
* ˵	����ASCII��A-F.0-9ת16����0x0A-0x0F.0x00-0x09
****************************************************************************/
u8 AsciiToHex(char ch)
{
	if(ch >= '0' && ch <= '9')	//����0~9
	{
		return (ch - 0x30);
	}
	else
	{
		if(ch >= 'A' && ch <= 'F')	//��ĸA~F
		{
			return (ch - 0x37);
		}
		else
		{
			return 0xFF;
		}
	}
}


/****************************************************************************
* ��	�ƣ�void GetCpuIdHex(void)
* ��	�ܣ���ȡCPUоƬIDֵ16����   
* ��ڲ�������
* ���ڲ�������
* ˵	����6��u16��ֵ���CPUID
			u16 x[6]={0};
****************************************************************************/
void GetCpuIdHex(u8* HCpuId)
{
	u32 CpuId[3]={0};

	CpuId[0] = (*(u32*)(0x1FFFF7E8));
	CpuId[1] = (*(u32*)(0x1FFFF7EC));
	CpuId[2] = (*(u32*)(0x1FFFF7F0));
	
	HCpuId[0] = (u8)(CpuId[0]>>24);
	HCpuId[1] = (u8)(CpuId[0]>>16); 
	HCpuId[2] = (u8)(CpuId[0]>>8);
	HCpuId[3] = (u8)(CpuId[0]);

	HCpuId[4] = (u8)(CpuId[1]>>24);
	HCpuId[5] = (u8)(CpuId[1]>>16); 
	HCpuId[6] = (u8)(CpuId[1]>>8);
	HCpuId[7] = (u8)(CpuId[1]);

	HCpuId[8] = (u8)(CpuId[2]>>24);
	HCpuId[9] = (u8)(CpuId[2]>>16); 
	HCpuId[10] = (u8)(CpuId[2]>>8);
	HCpuId[11] = (u8)(CpuId[2]);
}

/****************************************************************************
* ��	�ƣ�u8 JZ_chack(void)
* ��	�ܣ�8M������������ 
* ��ڲ�������
* ���ڲ�����u8	1����	0û����
* ˵    ����OSC_IN(5,12,23)	
			OSC_OUT(6,13,24)
* ���Դ���			
	flag=JZ_chack();
	if(flag==1)		//����
		flag=9;
	else			//û����
		flag=9;
****************************************************************************/
u8 JZ_chack(void)
{
	u8 flag=0;		 //������������ʱ

	RCC_DeInit();			 		//������ RCC�Ĵ�������Ϊȱʡֵ
	RCC_HSEConfig(RCC_HSE_ON);		//�����ⲿ8M���پ���(HSE)
									//RCC_HSE_OFF  		HSE 	���� OFF 
									//RCC_HSE_ON  		HSE 	���� ON 
									//RCC_HSE_Bypass  	HSE 	�����ⲿʱ����·
	flag = RCC_WaitForHSEStartUp();	//�ȴ� HSE ���񣬸ú������ȴ�ֱ�� HSE �����������ڳ�ʱ��������˳� 
	if(flag == SUCCESS)
		return 1;
	else
		return 0;
}

/****************************************************************************
* ��	�ƣ�void RCC_USEHSI(void)
* ��	�ܣ�����STM32�ڲ�RC����
* ��ڲ�������
* ���ڲ�������
* ˵	����
		���ʹ���ڲ�RC��������ʹ���ⲿ�����밴�����淽������
		1 ����100�Ż�144�ŵĲ�Ʒ��OSC_INӦ�ӵأ�OSC_OUTӦ���ա�
		2 ��������100�ŵĲ�Ʒ����2�ֽӷ���
			2.1 OSC_IN��OSC_OUT�ֱ�ͨ��10K����ӵء��˷��������EMC���ܡ�
			2.2 �ֱ���ӳ��OSC_IN��OSC_OUT��PD0��PD1��������PD0��PD1Ϊ������������'0'���˷������Լ�С���Ĳ�(�������2.1)��ʡ2���ⲿ���衣
* ��    �ԣ�
	u32 x=0;
	while(1)
	{
		printf("--- %d ---\r\n",x);
		x++;
		Delay_S(1);
	}
****************************************************************************/

/****************************************************************************
* ��	�ƣ�void System_Reset(void)
* ��	�ܣ�ϵͳ������
* ��ڲ�������
* ���ڲ�������
* ˵	����
		__enable_irq()  //���ж�
		__disable_irq() //���ж� 
* ��    �ԣ�
//������ʽ1��
	u8 x=0;
	printf("START\r\n");
	while(1)
	{
		printf("%d\r\n",x++);
		if(x==5)				//����10����
		{
			System_Reset();
			while(1){;}			//�����λʧ������������
		}
		Delay_S(1);
	}
	//__asm	���ָ��

//������ʽ2��	
	//NVIC_SystemReset();	//�̼����Դ���������
	u8 x=0;
	printf("START\r\n");
	while(1)
	{
		printf("%d\r\n",x++);
		if(x==5)				//����10����
		{
			__disable_irq();	//��ֹһ���жϲ��� 
			NVIC_SystemReset();
			while(1){;}			//�����λʧ������������
		}
		Delay_S(1);
	}
****************************************************************************/
__asm void System_Reset(void) 
{ 
	MOV R0, #1           //  
	MSR FAULTMASK, R0    // ���FAULTMASK ��ֹһ���жϲ��� 
	LDR R0, =0xE000ED0C  // 
	LDR R1, =0x05FA0004  //  
	STR R1, [R0]         // ϵͳ�����λ    
}

