#include "bsp.h"

/****************************************************************************
* 名	称：void  SysTickConfig(void)
* 功	能：系统时钟 ucos基时
* 入口参数：无
* 出口参数：无
* 说	明：无
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
* 名	称：void  BSP_Init(void)
* 功	能：驱动初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void BSP_Init(void)		//硬件资源初始化
{
	//SystemInit(); 			//STM32系统初始化,在启动文件中已经自动调用了
	//AT45DB321D_init();		//DataFlash	4M
}


