#include "tim2_init.h"
#include "timer.h"

//定时器中断检测开关 1开0关
#define TIM2_FullOut	0			  //溢出检测
#define TIM2_OC1		1			  //输出比较通道1检测
#define TIM2_OC2		1			  //输出比较通道2检测
#define TIM2_OC3		1			  //输出比较通道3检测
#define TIM2_OC4		1			  //输出比较通道4检测

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
* 名	称：void TIM2_Init(void)
* 功	能：定时器初始化
* 入口参数：无
* 出口参数：无
* 说	明：Get_SyS_CLK自行检测系统主频

3字节时间表	  参考设置USART1_Real.TimeVal延时值
115200= 1000000/11520 = 86us*3 = 258us/20us=计数12.9次	
57600= 1000000/5760 = 173us*3 = 519us/20us=计数25次
38400= 1000000/3840 = 260us*3 = 781us/20us=计数39次
19200= 1000000/1920 = 520us*3 = 1560us/20us=计数78次
9600= 1000000/960 = 1041us*3 = 3123us/20us=计数156次
****************************************************************************/
	u32 tim2_clock = 0;

void TIM2_Init(void)
{
	TIM_TimeBaseInitTypeDef  T;	//创建定时器变量

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	  	//TIM2使用APB1时钟总线
	TIM_DeInit(TIM2);											//复位TIM2定时器到默认值

	T.TIM_Period=500-1;								//计数器溢出值，最大65535，此值不能更改，与输出比较的设定值一致，输出比较计数器溢出也为65535

	if(get_APB1_prevdiv() > 0){
		tim2_clock = Get_SyS_CLK(3)<<1;
	}else{
		tim2_clock = Get_SyS_CLK(3);
	}

	T.TIM_Prescaler = tim2_clock/TIM2_PREV_DIV - 1; 	//预分频系数，系统每秒72,000,000次，72000000/50000=1440分频，对系统频率72000000进行1440分频后，每秒计数频率为50000次，即每次计数时间为20us，每计满TIM_Period多次时变0重计
	//T.TIM_Prescaler =7200 - 1; 	//预分频系数，系统每秒72,000,000次，72000000/50000=1440分频，对系统频率72000000进行1440分频后，每秒计数频率为50000次，即每次计数时间为20us，每计满TIM_Period多次时变0重计

	T.TIM_ClockDivision = 0;						//时钟分割 在定时器预分频之前作一段延时，很少用到，设为0即可
  	T.TIM_CounterMode = TIM_CounterMode_Up;			//向上计数		UP：0到10000的计数	DOWN:FFFF到10000 
  	TIM_TimeBaseInit(TIM2, &T);						//根据上面设置初始化定时器TIM2

  	TIM_ClearFlag(TIM2, TIM_FLAG_Update);			//清空溢出中断
   	TIM_SetCounter(TIM2,0);							//设置计数器初值为0	//目前尚未启用该定时器
}

/****************************************************************************
* 名	称：void TIM2_OC(void)
* 功	能：定时器输出比较功能初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/ 
void TIM2_OC(void)
{
	TIM_OCInitTypeDef O;  		//创建输出比较设置变量
	
    	O.TIM_OCMode = TIM_OCMode_Timing; 			  //配置成输出比较计时模式
   	O.TIM_OutputState = TIM_OutputState_Disable;  //禁止OC输出比较
    	O.TIM_OCPolarity = TIM_OCPolarity_Low; 		  //输出比较极为低

	O.TIM_Pulse = 0;		//初始化输出比较通道值	最大65535溢出，和TIM_Period一致

	TIM_OC1Init(TIM2, &O); 	//初始化输出比较通道1
	TIM_OC2Init(TIM2, &O); 	//初始化输出比较通道2
	TIM_OC3Init(TIM2, &O); 	//初始化输出比较通道3
	TIM_OC4Init(TIM2, &O); 	//初始化输出比较通道4	
}

/****************************************************************************
* 名	称：void TIM2_IC(void)
* 功	能：定时器输入捕获功能初始化
* 入口参数：无
* 出口参数：无
* 说	明：暂未实现
****************************************************************************/ 
void TIM2_IC(void)
{
	TIM_ICInitTypeDef I;  		//创建输出比较设置变量
	
	//输入捕获配置	TIMx_CCRx
	I.TIM_Channel = TIM_Channel_1; 				  	//使用输入捕获通道
	I.TIM_ICPolarity = TIM_ICPolarity_Rising; 	  	//输入捕获上升沿有效
	I.TIM_ICSelection = TIM_ICSelection_DirectTI; 	//IC*输入引脚选择，针对IC1/IC2有不同的定义
	I.TIM_ICPrescaler = TIM_ICPSC_DIV1; 		  	//输入模式下，捕获端口上的每一次边沿都触发一次捕获
	I.TIM_ICFilter = 0; 						  	//捕获采样频率，详见TIM*_CCMR->IC*F说明
	TIM_ICInit(TIM2, &I); 						  	//初始化输入
}

/****************************************************************************
* 名	称：void TIM2_NVIC_Init(void)
* 功	能：定时器中断向量表初始化
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/
void TIM2_NVIC_Init(void)
{
	NVIC_InitTypeDef N;
		  
	N.NVIC_IRQChannel = TIM2_IRQn;				//配置对TIM2_IRQn的中断监听
  	N.NVIC_IRQChannelPreemptionPriority = 0;	//抢先级
  	N.NVIC_IRQChannelSubPriority = 0;			//响应级
  	N.NVIC_IRQChannelCmd = ENABLE;				//开启中断监听
  	NVIC_Init(&N); 								//根据上面设置初始化中断控制器
}

/****************************************************************************
* 名	称：void TIM2_Config(void)
* 功	能：配置定时器
* 入口参数：无
* 出口参数：无
* 说	明：无
****************************************************************************/	   
void TIM2_Config(void)
{
	TIM2_Init();			   	//定时器初始化
	TIM2_NVIC_Init();			//中断向量表初始化
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

	base->CCMR1 = base->CNT + counter; 		//重新增加超时量
		
	if((base->DIER & tim_flag_cc) == 0)		//如果比较通道使能为关闭
	{				
		base->SR = (uint16_t)~tim_flag_cc; 	//清空比较通道1的中断标志 
		base->DIER |= tim_flag_cc;				//比较通道打开等待下一次中断
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
	
	TIM_ITConfig(base, tim_flag_cc, DISABLE);	//关闭中断

}

void TIM2_IRQHandler(void)		//TIM2中断处理 计数器不会因仿真器暂停而暂停
{
	//OSIntEnter();				//进入中断进行任务优先级调度保护

	if(TIM_GetITStatus(TIM2, TIM_FLAG_Update) != RESET) //溢出中断
 	{
 		do_timer(&(tim[0].tim_dev));
		//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);	 	//关闭中断
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);	//清除中断标志
	}
#if 0
	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC1) != RESET)	 //输出比较通道1中断
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);	//关闭中断
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_CC1);	//清除中断标志
		//raise_timeout(&tim[0]);
	}
	
	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC2) != RESET)	 //输出比较通道1中断
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC2, DISABLE);	//关闭中断
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_CC2);	//清除中断标志
	
		//raise_timeout(&tim[1]);
	
	}

	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC3) != RESET)	 //输出比较通道1中断
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE);	//关闭中断
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC2);	//清除中断标志
		
		//raise_timeout(&tim[2]);
		
	}

	if (TIM_GetITStatus(TIM2, TIM_FLAG_CC4) != RESET)	 //输出比较通道1中断
 	{
		//TIM_ITConfig(TIM2, TIM_IT_CC4, DISABLE);	//关闭中断
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_CC4);	//清除中断标志
		
		//raise_timeout(&tim[3]);
		
	}
#endif	
	//OSIntExit();	//退出中断进行任务优先级重新调度
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
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	  //开启溢出中断
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
