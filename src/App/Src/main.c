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
void net_work_test()
{
	mem_init();
	network_init();
	while(1)
		network_srv();

}
//======================================��ϵͳ������==================================
//��ʼ��Ӳ������ʼ��ϵͳ��������������				
int main(void)
{	
#if 0
	net_work_test();
#endif
	nvic_init();
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


//��������============================================================================
void StartJob(void ) 
{
		
	struct TimerDevice* dev = request_timer(1);
	timer_device_enable(dev,true);
	while(1)
	{
		timer_srv();
		#if (DEVICE_NETWORK_EN > 0)
			network_srv();
		#endif
	}

}

