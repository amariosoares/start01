#include "includes.h"
#include "rtc_stm32.h"
#include "datetime.h"
#if (DEVICE_STM32_RTC_EN > 0)
#define DEV_NAME "rtc_stm32"

RTC_Device stm32rtc;

static u32 GetRtcCounter(void);
static void SetRtcCounter(u32 nValue);
static void RTC_Configuration(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
// 		/* 允许访问 BKP 区域 */
	PWR_BackupAccessCmd(ENABLE);
// 		
	/* 使能 LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* 等待 LSE 稳定 */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* 选择 LSE 作为 RTC 时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* 使能 RTC 时钟 */
	RCC_RTCCLKCmd(ENABLE);

	/* 等待 RTC 寄存器同步 */
	RTC_WaitForSynchro();

	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();

	/* 使能 RTC 秒中断 */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();

	/* 设置 RTC 预分频为一秒 */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
	 
}


static bool stm32_get_datetime(TDateTime *dt)
{
	if(dt){
		datetime_convert(GetRtcCounter(),dt);
		
		return true;
	}
	return false;
}
static bool stm32_set_datetime(TDateTime *dt)
{
	if(dt){		
		SetRtcCounter(datetime_mktime(dt));
		
		return true;
	}
	return false;
}
/**************************************************
**函数原型:	void ResetRTCConter(void)
**功能:		复位实时时钟计数器
**输入参数:	无
**返回参数:	无
**************************************************/
static void ResetRtcCounter(void)
{
	/* 读设置RTC计数器为0*/
	RTC_SetCounter(0x00);

	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
}

/**************************************************
**函数原型:	uint32 GetRTCCounter(void)
**功能:		获取实时时钟计数器当前值
**输入参数:	无
**返回参数:	实时时钟计数器当前值
**************************************************/
static u32 GetRtcCounter(void)
{
	u32 nValue;
	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
	/* 读取RTC计数器*/
	nValue = RTC_GetCounter();
	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
	
	return nValue;
}

/**************************************************
**函数原型:	void SetRTCCounter(uint32 nValue)
**功能:		设置实时时钟计数器当前值
**输入参数:	无
**返回参数:	无
**************************************************/
static void SetRtcCounter(u32 nValue)
{
	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
	/* 读取RTC计数器*/
	RTC_SetCounter(nValue);
	/* 等待最后一次RTC寄存器操作完成*/
	RTC_WaitForLastTask();
}
static void rtc_hard_init(void)
{
	RTC_Configuration();
}
static int stm32_rtc_init()
{
	

	rtc_hard_init();
	
	memset(&stm32rtc, 0 , sizeof(RTC_Device));
	
	strncpy(stm32rtc.name,DEV_NAME,MAX_RTC_DEV_NAME);
	
	stm32rtc.get_datetime = stm32_get_datetime;
	stm32rtc.set_datetime = stm32_set_datetime;
	
	register_rtc(&stm32rtc);
	
	return 1;
}
device_initcall(stm32_rtc_init);
#endif
