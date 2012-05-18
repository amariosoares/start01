#include "includes.h"
#include "watchdog.h"
#include "timer.h"

#define WDT_B_HIGH	    			(GPIOB->BSRR = GPIO_Pin_1)
#define WDT_B_LOW					(GPIOB->BRR  = GPIO_Pin_1)

void watchdog_feed(void)
{
	
		/*Î¹¹·*/
	if(GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_1)==0)//GPIO_ReadInputDataBit		  
		WDT_B_HIGH;
	else
		WDT_B_LOW;
	
}
void watchdog_task(void* tid, void* arg)
{
	watchdog_feed();
	led_revert(1);
}

static int watchdog_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	struct TimerDevice* dev = NULL;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		
	GPIO_InitStructure.GPIO_Pin 	=  GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);		

	dev = request_timer(1);
	if(dev == NULL) return 0;
	
	if( dev ){
		if(!create_timer(dev, 500*DELAY_MSECOND, watchdog_task, NULL, LOOP))
			return 0;
	}

	
}
device_initcall(watchdog_init);

