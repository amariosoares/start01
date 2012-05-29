#include "main.h"	 //������Դ
#include "Startup.h"
#include "serial.h"
#include "timer.h"
#include "network.h"

//======================================�������塿==================================
#define  StartJob_STK_SIZE		128      	//���������ջ��С
#define  SYSCLK_FREQ_72MHz  72000000
static   bool SysClockCheck(void);



//������
void StartJob			(void);			//��������
			  
//======================================�����������==================================
extern u8 HardFault_Handler_T;	//�Ҵ�
#if (DEVICE_NETWORK_EN > 0)
static void net_work_test()
{
	mem_init();
	network_init();
	while(1)
		network_srv();

}
#endif
//======================================��ϵͳ������==================================
//��ʼ��Ӳ������ʼ��ϵͳ��������������				
int main(void)
{	
#if 0
	net_work_test();
#endif
	BSP_Init();
	InitSystem();
	if(!SysClockCheck()){
		DebugPf("System Check Failed\r\n");
		while(1){}
	}
	StartJob();
	return 0;
}
static bool SysClockCheck(void)
{
	
	RCC_ClocksTypeDef RCC_ClockFreq;

	RCC_GetClocksFreq(&RCC_ClockFreq);	//Ϊʱ�ӱ�������Ƭ�ϸ�ʱ��

	if(SYSCLK_FREQ_72MHz != RCC_ClockFreq.SYSCLK_Frequency)	//SYSTEMƵ��
	{
		return false;
	}	
	return true;
}

extern void anybus_service(void);
//��������============================================================================
void StartJob(void ) 
{
	start_sys_timer();
	while(1)
	{
		timer_srv();
		#if (DEVICE_NETWORK_EN > 0)
			network_srv();
		#endif
		anybus_service();
	}

}

