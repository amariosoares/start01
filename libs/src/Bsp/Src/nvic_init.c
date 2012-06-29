#include <nvic_init.h>

/****************************************************************************
* 名	称：void nvic_init(void)
* 功	能：中断管理初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void nvic_init(void)
{
	#ifdef  VECT_TAB_RAM	//中断向量表存放地址到SRAM 0x20000000  
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else					//中断向量表存放地址到FLASH	0x08000000
  	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xA000);   
  
	#endif
}
