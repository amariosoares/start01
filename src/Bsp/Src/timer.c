#include "includes.h"
#include "timer.h"

//定时器中断检测开关 1开0关
#define TIM2_FullOut	0			  //溢出检测
#define TIM2_OC1		1			  //输出比较通道1检测
#define TIM2_OC2		1			  //输出比较通道2检测
#define TIM2_OC3		1			  //输出比较通道3检测
#define TIM2_OC4		1			  //输出比较通道4检测


#define MAX_TIMER_CNT 4
static struct TimerDevice*  timer_dev_list[MAX_TIMER_CNT];
static u8 max_id	 = 0;
static u8 current_id = 0;
#define IRQ_LOCK
#define IRQ_UNLOCK
static void  timer_var_init(TID timer);

void    timer_device_set_period(struct TimerDevice* dev, u32 period_us)
{
	if(dev){
		dev->cfg.period_us = period_us;
		dev->set_irq_peroid(dev,period_us);
	}
}

u32     timer_device_get_period(struct TimerDevice* dev)
{
	if(dev){
		return dev->cfg.period_us;
	}
	return 0;
}

BOOL register_timer_irq_hook(struct TimerDevice* dev,u32 timeout_us,timer_irq_callback cb,void* arg)
{
	if(dev){
		dev->cfg.dev_arg	   = arg;
		dev->cfg.irq_callback = cb;
		if(timeout_us < dev->cfg.period_us)  timeout_us = dev->cfg.period_us;
			
		dev->cfg.irq_cb_period_us = timeout_us;
		dev->register_timer_irq_hook(dev,timeout_us,cb,arg);
		return TRUE;
	}
	return FALSE;
}
BOOL unregister_timer_irq_hook(struct TimerDevice* dev)
{
		if(dev){
			dev->cfg.dev_arg	   =  NULL;
			dev->cfg.irq_callback = NULL;	
			dev->cfg.irq_cb_period_us = 0;
			dev->unregister_timer_irq_hook(dev);
		return TRUE;
	}
}
void  timer_device_enable(struct TimerDevice* dev,BOOL enable)
{
	if(dev){
		dev->enable(dev,enable);
	}
}

u32 	 timer_num(struct TimerDevice* dev)
{
	if(dev){
		return dev->cfg.timer_num;
	}
	return 0;
}
void timer_init_cfg(TimerDeviceCfg* cfg,u32 period_us)
{
	if(cfg){
		cfg->irq_callback = NULL;
		cfg->dev_arg = NULL;
		cfg->period_us = period_us;
		for(int i = 0; i < MAX_TIMER_NUM; i++)
		{
			timer_var_init(cfg->timer_list+i);
		}
		
	}
}
static void  timer_var_init(TID timer)
{
	if(timer){
		timer->used = 0;
		timer->flag = ONE_TIME;
		timer->fn    = NULL;
		timer->reload_timeout_us = timer->timeout_us = 0;
		timer->tid = 0;
		timer->private_arg = NULL;
	}
}
static BOOL  get_free_timer_id(struct TimerDevice* dev,u8* id)
{		
	u8 i;
	for(i = 0; i < MAX_TIMER_NUM; i++)
	{
		if(!(dev->cfg.timer_list[i].used))
			break;
	}
	if(i== MAX_TIMER_NUM)  return FALSE;
	*id = i;
	return TRUE;
}
TID 	create_timer(struct TimerDevice* dev, u32 timeout_us,TIME_OUT_PROC timer_cb, void* arg,u8 flag)
{
	if(dev && (dev->cfg.timer_num < MAX_TIMER_NUM)){
		u8  id = 0; 
		TID  timer = NULL;
IRQ_LOCK
		if(!get_free_timer_id(dev,&id)) return NULL;

		
		dev->cfg.timer_list[id].flag = flag;
		dev->cfg.timer_list[id].fn    = timer_cb;
		dev->cfg.timer_list[id].reload_timeout_us = timeout_us;
		dev->cfg.timer_list[id].timeout_us = timeout_us;

		dev->cfg.timer_list[id].tid  = id;
		dev->cfg.timer_list[id].private_arg = arg;
		dev->cfg.timer_list[id].suspend = 1;
		dev->cfg.timer_list[id].used = 1;
		timer = dev->cfg.timer_list+id;
		dev->cfg.timer_num++;
IRQ_UNLOCK		
		return timer;
	}
}
BOOL    timer_reset(TID timer)
{
	if(timer){
		if(timer->flag == LOOP)
			timer->timeout_us = timer->reload_timeout_us;
		return TRUE;
	}
	return FALSE;
}

void	timer_start(struct TimerDevice* dev,TID timer)
{
	IRQ_LOCK

	if(timer){
		if(dev){
			timer->suspend = 0;
			if(dev->start)
				dev->start(dev,timer);
		}
	}
	IRQ_UNLOCK		
	
}
void	timer_stop(struct TimerDevice* dev,TID timer)
{
	IRQ_LOCK

	if(timer){
		if(dev){
			timer->suspend = 1;
			dev->stop(dev,timer);
		}
	}
	
	IRQ_UNLOCK			
}

struct TimerDevice*  	 request_timer(u8 num)
{
	if(num >  max_id) 
		return NULL;
	return timer_dev_list[num-1];
}



void raise_timeout(struct TimerDevice* dev)
{

}
u8        timer_device_num(void)
{
	return current_id;
}
BOOL register_timer_device(struct TimerDevice* dev)
{
	if(max_id  >= MAX_TIMER_CNT) return FALSE;

	if(dev){
		timer_dev_list[max_id++] = dev;
		return TRUE;

	}
	return FALSE;
}
 void timer_srv(void)
{
	
	for(int i = 0; i < max_id; i++){

		if(timer_dev_list[i]->irq_flag && (timer_dev_list[i]->cfg.timer_num > 0))
		{
			ptr_timer timer;
			timer = timer_dev_list[i]->cfg.timer_list;
			for(int j = 0; j < MAX_TIMER_NUM; j++)
			{
				if(timer[j].used && timer[j].timeout_us <= 0)
				{
					if(NULL==timer[j].fn) 
						continue;
					
					if(timer[j].flag == ONE_TIME){
						timer_var_init(timer+j);
					}else if(timer[j].flag == LOOP){
						timer[j].timeout_us = timer[j].reload_timeout_us;
					}
					timer[j].fn(timer_dev_list + j, timer[j].private_arg);
				}
			}
		}
		timer_dev_list[i]->irq_flag = 0;

	}
}
void do_timer(struct TimerDevice* dev)
{
	//TimerDeviceCfg* cfg = &(dev->cfg);
	ptr_timer item = dev->cfg.timer_list;
	u32 period = dev->cfg.period_us;
	for(int i = 0; i < MAX_TIMER_NUM; i++)
	{
		if(item[i].used && (item[i].timeout_us > 0)){
			item[i].timeout_us  -= period;
		}
	}
	dev->irq_flag = 1;
}
BOOL start_sys_timer(void)
{
	struct TimerDevice* dev = request_timer(1);
	timer_device_enable(dev,true);

	return (dev!=NULL);

}
static int timer_init(void)
{
	memset(timer_dev_list, 0, ARRAY_SIZE(timer_dev_list));
	current_id = max_id = 0;
	
	return 1;
}
core_initcall(timer_init);
