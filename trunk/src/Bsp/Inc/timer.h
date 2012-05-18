#ifndef TIMER_H
#define TIMER_H


#define ONE_TIME 0
#define LOOP     1 
#define MAX_TIMER_NUM  8

#define SUSPEND_TIMER  0
#define RESUME_TIMER   1


typedef void (*TIME_OUT_PROC)(void* tid, void* arg);
typedef struct 
{
	u8              tid;
	u8	         used;
	u8              flag; //0:一次 1:循环
	u8              suspend; //挂起的标志
	long 	         reload_timeout_us; /**< 中断间隔. */
	long            timeout_us; //  
	TIME_OUT_PROC   fn; //超时处理函数
	void*        private_arg;
	struct       ptr_timer*         next; 
}timer,*ptr_timer;
typedef ptr_timer TID;

typedef enum{
	ID_TIMER1=1,
	ID_TIMER2,
	ID_TIMER3,
	ID_TIMER4,
	ID_TIMER5,
	ID_TIMER6,
}TimerID;
typedef void (*timer_irq_callback)(void* dev,void* arg);


typedef struct{
	u32    timer_num;
	u32    period_us;
	u32    irq_cb_period_us;
	timer_irq_callback irq_callback;
	timer timer_list[MAX_TIMER_NUM];
	void* dev_arg;
	
}TimerDeviceCfg;

struct TimerDevice{
	char name[16];
	u8   id;
//===========real timer device================	
	u32     (*get_irq_peroid)(struct TimerDevice* dev);	
	u32     (*set_irq_peroid)(struct TimerDevice* dev,u32 period);
	BOOL (*register_timer_irq_hook)(struct TimerDevice* dev,u32 timeout_us,timer_irq_callback cb,void* arg);
	BOOL (*unregister_timer_irq_hook)(struct TimerDevice* dev);
	/*@
		允许或禁用定时器设备
		return 
			TRUE :  启动成功
			FALSE:  启动失败
	*/

	void   (*enable)(struct TimerDevice* dev,BOOL enable);
//=========== abstract timer ================	
	/*@
		获取某个定时器设备支持的应用定时器的个数
		return 
			TRUE :  启动成功
			FALSE:  启动失败
	*/

	u32     (*get_timer_num)(struct TimerDevice* dev);
	void    (*stop)(struct TimerDevice* dev,TID timer);
	/*@
		启动指定应用定时器
		return 
			TRUE :  启动成功
			FALSE:  启动失败
	*/
	BOOL (*start)(struct TimerDevice* dev,TID timer);
	/*@
		申请一个应用定时器
		return TID
			NULL :  failed
			!NULL: 定时器对象
	*/
	TID     (*request_timer)(struct TimerDevice* dev, u32 timeout_us,TIME_OUT_PROC timer_cb, 
	void* arg,u8 flag);
	/*@
		释放一个应用定时器
	*/
	void   (*release_timer)(struct TimerDevice* dev,TID timer);
//============timer config =================	
	TimerDeviceCfg cfg;
	void* private_data;
	volatile u32    irq_flag;
};

#define DELAY_SECOND 		((u32)(1000  * 1000))
#define DELAY_MSECOND 	((u32)(1000 ))


struct TimerDevice* 	 request_timer(u8 num);
void    timer_device_set_period(struct TimerDevice* dev, u32 period_us);
u32     timer_device_get_period(struct TimerDevice* dev);
BOOL register_timer_irq_hook(struct TimerDevice* dev,u32 timeout_us,timer_irq_callback cb,void* arg);
BOOL unregister_timer_irq_hook(struct TimerDevice* dev);
void   timer_device_enable(struct TimerDevice* dev,BOOL enable);
//=======================================
u32 	 timer_num(struct TimerDevice* dev);
TID 	create_timer(struct TimerDevice* dev, u32 timeout_us,TIME_OUT_PROC timer_cb, void* arg,u8 flag);
BOOL    timer_reset(TID timer);
void	timer_start(struct TimerDevice* dev,TID timer);
void	timer_stop(struct TimerDevice* dev,TID timer);

BOOL register_timer_device(struct TimerDevice* dev);

u8     timer_device_num(void);
void timeout_raise(struct TimerDevice* dev);
void timer_init_cfg(TimerDeviceCfg* cfg,u32 period_us);

void do_timer(struct TimerDevice* dev);
extern  void timer_srv(void);
#endif
