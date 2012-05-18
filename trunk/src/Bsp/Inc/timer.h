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
	u8              flag; //0:һ�� 1:ѭ��
	u8              suspend; //����ı�־
	long 	         reload_timeout_us; /**< �жϼ��. */
	long            timeout_us; //  
	TIME_OUT_PROC   fn; //��ʱ������
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
		�������ö�ʱ���豸
		return 
			TRUE :  �����ɹ�
			FALSE:  ����ʧ��
	*/

	void   (*enable)(struct TimerDevice* dev,BOOL enable);
//=========== abstract timer ================	
	/*@
		��ȡĳ����ʱ���豸֧�ֵ�Ӧ�ö�ʱ���ĸ���
		return 
			TRUE :  �����ɹ�
			FALSE:  ����ʧ��
	*/

	u32     (*get_timer_num)(struct TimerDevice* dev);
	void    (*stop)(struct TimerDevice* dev,TID timer);
	/*@
		����ָ��Ӧ�ö�ʱ��
		return 
			TRUE :  �����ɹ�
			FALSE:  ����ʧ��
	*/
	BOOL (*start)(struct TimerDevice* dev,TID timer);
	/*@
		����һ��Ӧ�ö�ʱ��
		return TID
			NULL :  failed
			!NULL: ��ʱ������
	*/
	TID     (*request_timer)(struct TimerDevice* dev, u32 timeout_us,TIME_OUT_PROC timer_cb, 
	void* arg,u8 flag);
	/*@
		�ͷ�һ��Ӧ�ö�ʱ��
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
BOOL start_sys_timer(void);
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
