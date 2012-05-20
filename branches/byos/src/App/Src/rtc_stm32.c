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
// 		/* ������� BKP ���� */
	PWR_BackupAccessCmd(ENABLE);
// 		
	/* ʹ�� LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* �ȴ� LSE �ȶ� */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}

	/* ѡ�� LSE ��Ϊ RTC ʱ��Դ */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* ʹ�� RTC ʱ�� */
	RCC_RTCCLKCmd(ENABLE);

	/* �ȴ� RTC �Ĵ���ͬ�� */
	RTC_WaitForSynchro();

	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();

	/* ʹ�� RTC ���ж� */
	RTC_ITConfig(RTC_IT_SEC, ENABLE);

	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();

	/* ���� RTC Ԥ��ƵΪһ�� */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* �ȴ����һ��RTC�Ĵ����������*/
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
**����ԭ��:	void ResetRTCConter(void)
**����:		��λʵʱʱ�Ӽ�����
**�������:	��
**���ز���:	��
**************************************************/
static void ResetRtcCounter(void)
{
	/* ������RTC������Ϊ0*/
	RTC_SetCounter(0x00);

	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();
}

/**************************************************
**����ԭ��:	uint32 GetRTCCounter(void)
**����:		��ȡʵʱʱ�Ӽ�������ǰֵ
**�������:	��
**���ز���:	ʵʱʱ�Ӽ�������ǰֵ
**************************************************/
static u32 GetRtcCounter(void)
{
	u32 nValue;
	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();
	/* ��ȡRTC������*/
	nValue = RTC_GetCounter();
	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();
	
	return nValue;
}

/**************************************************
**����ԭ��:	void SetRTCCounter(uint32 nValue)
**����:		����ʵʱʱ�Ӽ�������ǰֵ
**�������:	��
**���ز���:	��
**************************************************/
static void SetRtcCounter(u32 nValue)
{
	/* �ȴ����һ��RTC�Ĵ����������*/
	RTC_WaitForLastTask();
	/* ��ȡRTC������*/
	RTC_SetCounter(nValue);
	/* �ȴ����һ��RTC�Ĵ����������*/
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
