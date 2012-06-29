#include "tim2_init.h"
#include "timer.h"

//��ʱ���жϼ�⿪�� 1��0��
#define TIM2_FullOut	0			  //������
#define TIM2_OC1		1			  //����Ƚ�ͨ��1���
#define TIM2_OC2		1			  //����Ƚ�ͨ��2���
#define TIM2_OC3		1			  //����Ƚ�ͨ��3���
#define TIM2_OC4		1			  //����Ƚ�ͨ��4���

#define TIM2_PERIOD_US   5000L //5000us
#define TIM2_UNIT_US 	 10  //10us
#define TIM2_PREV_DIV      (1000000L/TIM2_UNIT_US)
#define TIM2_COUNTER  (TIM2_PERIOD_US/TIM2_UNIT_US)
typedef struct{
	int                                       tim_chan;
	TIM_TypeDef*		 tim_base;
	struct TimerDevice   tim_dev;
}stm32_timer;
#if 1
static stm32_timer  tim[] = {
		{.tim_chan = 0,.tim_base = TIM2},
};
#endif
/****************************************************************************
* ��	�ƣ�void TIM2_Init(void)
* ��	�ܣ���ʱ����ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	����Get_SyS_CLK���м��ϵͳ��Ƶ

3�ֽ�ʱ���	  �ο�����USART1_Real.TimeVal��ʱֵ
115200= 1000000/11520 = 86us*3 = 258us/20us=����12.9��	
57600= 1000000/5760 = 173us*3 = 519us/20us=����25��
38400= 1000000/3840 = 260us*3 = 781us/20us=����39��
19200= 1000000/1920 = 520us*3 = 1560us/20us=����78��
9600= 1000000/960 = 1041us*3 = 3123us/20us=����156��
****************************************************************************/
	u32 tim2_clock = 0;

void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef  T;	//������ʱ������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	  	//TIM2ʹ��APB1ʱ������
	TIM_DeInit(TIM2);											//��λTIM2��ʱ����Ĭ��ֵ

	T.TIM_Period=500-1;								//���������ֵ�����65535����ֵ���ܸ��ģ�������Ƚϵ��趨ֵһ�£�����Ƚϼ��������ҲΪ65535

	if(get_APB1_prevdiv() > 0){
		tim2_clock = Get_SyS_CLK(3)<<1;
	}else{
		tim2_clock = Get_SyS_CLK(3);
	}

	T.TIM_Prescaler = tim2_clock/TIM2_PREV_DIV - 1; 	//Ԥ��Ƶϵ����ϵͳÿ��72,000,000�Σ�72000000/50000=1440��Ƶ����ϵͳƵ��72000000����1440��Ƶ��ÿ�����Ƶ��Ϊ50000�Σ���ÿ�μ���ʱ��Ϊ20us��ÿ����TIM_Period���ʱ��0�ؼ�
	//T.TIM_Prescaler =7200 - 1; 	//Ԥ��Ƶϵ����ϵͳÿ��72,000,000�Σ�72000000/50000=1440��Ƶ����ϵͳƵ��72000000����1440��Ƶ��ÿ�����Ƶ��Ϊ50000�Σ���ÿ�μ���ʱ��Ϊ20us��ÿ����TIM_Period���ʱ��0�ؼ�

	T.TIM_ClockDivision = 0;						//ʱ�ӷָ� �ڶ�ʱ��Ԥ��Ƶ֮ǰ��һ����ʱ�������õ�����Ϊ0����
  	T.TIM_CounterMode = TIM_CounterMode_Up;			//���ϼ���		UP��0��10000�ļ���	DOWN:FFFF��10000 
  	TIM_TimeBaseInit(TIM2, &T);						//�����������ó�ʼ����ʱ��TIM2

  	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			//�������ж�
   	TIM_SetCounter(TIM2,0);							//���ü�������ֵΪ0	//Ŀǰ��δ���øö�ʱ��
}

/****************************************************************************
* ��	�ƣ�void TIM2_OC(void)
* ��	�ܣ���ʱ������ȽϹ��ܳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/ 
void TIM2_OC(void)
{
	TIM_OCInitTypeDef O;  		//��������Ƚ����ñ���
	
    	O.TIM_OCMode = TIM_OCMode_Timing; 			  //���ó�����Ƚϼ�ʱģʽ
   	O.TIM_OutputState = TIM_OutputState_Disable;  //��ֹOC����Ƚ�
    	O.TIM_OCPolarity = TIM_OCPolarity_Low; 		  //����Ƚϼ�Ϊ��

	O.TIM_Pulse = 0;		//��ʼ������Ƚ�ͨ��ֵ	���65535�������TIM_Periodһ��

	TIM_OC1Init(TIM2, &O); 	//��ʼ������Ƚ�ͨ��1
	TIM_OC2Init(TIM2, &O); 	//��ʼ������Ƚ�ͨ��2
	TIM_OC3Init(TIM2, &O); 	//��ʼ������Ƚ�ͨ��3
	TIM_OC4Init(TIM2, &O); 	//��ʼ������Ƚ�ͨ��4	
}

/****************************************************************************
* ��	�ƣ�void TIM2_IC(void)
* ��	�ܣ���ʱ�����벶���ܳ�ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������δʵ��
****************************************************************************/ 
void TIM2_IC(void)
{
	TIM_ICInitTypeDef I;  		//��������Ƚ����ñ���
	
	//���벶������	TIMx_CCRx
	I.TIM_Channel = TIM_Channel_1; 				  	//ʹ�����벶��ͨ��
	I.TIM_ICPolarity = TIM_ICPolarity_Rising; 	  	//���벶����������Ч
	I.TIM_ICSelection = TIM_ICSelection_DirectTI; 	//IC*��������ѡ�����IC1/IC2�в�ͬ�Ķ���
	I.TIM_ICPrescaler = TIM_ICPSC_DIV1; 		  	//����ģʽ�£�����˿��ϵ�ÿһ�α��ض�����һ�β���
	I.TIM_ICFilter = 0; 						  	//�������Ƶ�ʣ����TIM*_CCMR->IC*F˵��
	TIM_ICInit(TIM2, &I); 						  	//��ʼ������
}

/****************************************************************************
* ��	�ƣ�void TIM2_NVIC_Init(void)
* ��	�ܣ���ʱ���ж��������ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void TIM2_NVIC_Init(void)
{
	NVIC_InitTypeDef N;
		  
	N.NVIC_IRQChannel = TIM2_IRQn;				//���ö�TIM2_IRQn���жϼ���
  	N.NVIC_IRQChannelPreemptionPriority = 0;	//���ȼ�
  	N.NVIC_IRQChannelSubPriority = 0;			//��Ӧ��
  	N.NVIC_IRQChannelCmd = ENABLE;				//�����жϼ���
  	NVIC_Init(&N); 								//�����������ó�ʼ���жϿ�����
}

/****************************************************************************
* ��	�ƣ�void TIM2_Config(void)
* ��	�ܣ����ö�ʱ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/	   
void TIM2_Config(void)
{
	TIM2_Init();			   	//��ʱ����ʼ��
	TIM2_NVIC_Init();			//�ж��������ʼ��
}


static BOOL tim_start_timer(struct TimerDevice* timer,u32 timeout_us)
{
	if(timer==NULL || timer->private_data )
		return FALSE;
	
	stm32_timer* tim = timer->private_data;
	if(tim->tim_chan >=4 ) return FALSE;
	
	TIM_TypeDef* base = tim->tim_base;
	u16  counter = timeout_us /timer->cfg.period_us;
	u16 tim_flag_cc = (2<<(tim->tim_chan));

	base->CCMR1 = base->CNT + counter; 		//�������ӳ�ʱ��
		
	if((base->DIER & tim_flag_cc) == 0)		//����Ƚ�ͨ��ʹ��Ϊ�ر�
	{				
		base->SR = (uint16_t)~tim_flag_cc; 	//��ձȽ�ͨ��1���жϱ�־ 
		base->DIER |= tim_flag_cc;				//�Ƚ�ͨ���򿪵ȴ���һ���ж�
	}
	return TRUE;
}
static void tim_stop_timer(struct TimerDevice* timer)
{
	
	if(timer==NULL || timer->private_data)return ;

	stm32_timer* tim = timer->private_data;
	
	if(tim->tim_chan >=4 ) return;
	TIM_TypeDef* base = tim->tim_base;
	
	u16 tim_flag_cc = (2<<tim->tim_chan);
	
	TIM_ITConfig(base, tim_flag_cc, DISABLE);	//�ر��ж�

}

void TIM2_IRQHandler(void)		//TIM2�жϴ��� �������������������ͣ����ͣ
{
	//OSIntEnter();				//�����жϽ����������ȼ����ȱ���

	if(TIM_GetITStatus(TIM2, TIM_FLAG_Update) != RESET) //����ж�
 	{
 		do_timer(&(tim[0].tim_dev));
		//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);	 	//�ر��ж�
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);	//����жϱ�־
	}
#if 0
	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC1) != RESET)	 //����Ƚ�ͨ��1�ж�
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_CC1);	//����жϱ�־
		//raise_timeout(&tim[0]);
	}
	
	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC2) != RESET)	 //����Ƚ�ͨ��1�ж�
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_CC2);	//����жϱ�־
	
		//raise_timeout(&tim[1]);
	
	}

	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC3) != RESET)	 //����Ƚ�ͨ��1�ж�
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC2);	//����жϱ�־
		
		//raise_timeout(&tim[2]);
		
	}

	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC4) != RESET)	 //����Ƚ�ͨ��1�ж�
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);	//�ر��ж�
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC4);	//����жϱ�־
		
		//raise_timeout(&tim[3]);
		
	}
#endif	
	//OSIntExit();	//�˳��жϽ����������ȼ����µ���
}

u32 	tim_get_irq_peroid(struct TimerDevice* dev)
{
	   return TIM2_PERIOD_US;
}
u32 	tim_set_irq_peroid(struct TimerDevice* dev,u32 period)
{
	return 0;
}

void tim_enable_timer(struct TimerDevice* dev,BOOL enable)
{
	if(enable){
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	  //��������ж�
		TIM_Cmd(TIM2, ENABLE);	
	}else{
		TIM_Cmd(TIM2, DISABLE);	
	}
}

static int timer_init(void)
{
	
	for(int i = 0; i < 1; i++){
	
		tim[i].tim_dev.enable = tim_enable_timer;

		timer_init_cfg(&(tim[i].tim_dev.cfg),TIM2_PERIOD_US);
		register_timer_device(&(tim[i].tim_dev));
	}
	
	TIM2_Config();
	return 1;
	
}
coredevice_initcall(timer_init);
