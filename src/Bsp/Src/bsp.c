#include "bsp.h"

/****************************************************************************
* ��	�ƣ�void  SysTickConfig(void)
* ��	�ܣ�ϵͳʱ�� ucos��ʱ
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void SysTickConfig(void)
{
	u32 cnts;
    RCC_ClocksTypeDef  rcc_clocks;

	RCC_GetClocksFreq(&rcc_clocks);
    cnts = (u32)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(cnts);   //OS_TICKS_PER_SEC*(SystemFrequency/1000)
}

static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

u8 get_APB1_prevdiv()
{
	
#define CFGR_PPRE1_Set_Mask       ((uint32_t)0x00000700)
	/* Get PCLK1 prescaler */
	u32 tmp = RCC->CFGR & CFGR_PPRE1_Set_Mask;

	
	tmp = tmp>>8;
	
	return  APBAHBPrescTable[tmp];

}

/****************************************************************************
* ��	�ƣ�void  BSP_Init(void)
* ��	�ܣ�������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void BSP_Init(void)		//Ӳ����Դ��ʼ��
{
	//SystemInit(); 			//STM32ϵͳ��ʼ��,�������ļ����Ѿ��Զ�������
	//AT45DB321D_init();		//DataFlash	4M
}


