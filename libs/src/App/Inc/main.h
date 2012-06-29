#ifndef _MAIN_INIT_H_	
#define _MAIN_INIT_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

#include "bsp.h"	 		//硬件资源
#include "includes.h"		//系统资源

#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif


/*信号量例子	 //未处理多次发送信号量会叠加
OS_EVENT *Signal = 0;		 			//定义任务二信号量 
if(Signal == NULL) 
	Signal=OSSemCreate(0); 				//创建任务二信号量
	 
u8 Task2xherror=0;		  				//等待任务二信号量接收错误标志

OSSemPend(Task2xh,0,&Task2xherror);		//挂起等待任务二信号量
OSSemPost(Task2xh); 					//激活任务二信号量 
*/



/*线程挂起
OSTaskSuspend(2);						//将该线程挂起
OSTaskResume(2);						//将该线程从挂起中恢复
OSTaskSuspend(OS_PRIO_SELF);
*/



/*邮箱			 //未处理多次发送后几次发送会返回OS_ERR_MBOX_FULL错误，但是邮件携带地址会改变
#define OS_MBOX_EN                1    	//os_cfg.h 打开ucosii邮箱功能

OS_EVENT *MailBox = 0;						//定义邮箱信号量
if(MailBox == NULL)
	MailBox= OSMboxCreate((void *)0);		//创建邮箱信号量

u8 err;									//邮箱接收错误标志
u8 z[9]={0};							//保存邮箱传来的指针所指数据
void *y;								//邮箱传来的地址指针

y=OSMboxPend(MailBox, 0, &err);			//邮箱接收挂起

memcpy(z,(u8*)y,9);						//接收到邮箱之后将指针所指地址上的数据取出
u8 x[9]={1,2,3,4,5,6,7,8,9};			//需要用邮箱发送的数据

OSMboxPost(MailBox, (void *)&x);		//发送激活等待的邮箱
*/



/*开关中断原子操作
#if OS_CRITICAL_METHOD == 3             //开始部分引用            
    OS_CPU_SR  cpu_sr;
#endif
OS_ENTER_CRITICAL();					//开启中断失能功能
OS_EXIT_CRITICAL();						//关闭中断使能功能
*/



/*互斥量
#define OS_MUTEX_EN               1     //os_cfg.h 打开ucosii互斥量功能
OS_EVENT *Mutex = 0;		 			//定义互斥量

if(Mutex == NULL)
	Mutex = OSMutexCreate(2, &err);		//创建互斥量  //提升到的优先级，该优先级不能被占用

OSMutexPend(Mutex, 0, &err);			//获取互斥量

OSMutexPost(Mutex);						//释放互斥量
*/



/*消息队列
#define OS_Q_EN                   1		//os_cfg.h 打开消息队列功能
#define OS_MAX_QS                10		//os_cfg.h 修改队列控制块数量

#define QueueSize 6						//消息最大数量
OS_EVENT *Queue = 0;					//定义消息队列信号量
void* QueueBuff[QueueSize];	   			//消息队列缓冲区

if(Queue == NULL)
	Queue = OSQCreate(QueueBuff, QueueSize);    //创建消息队列信号量

void* pMsg;				//消息带来的值地址
u8 err;					//队列消息状态
u8 Y=0;					//消息带来的数据
pMsg = OSQPend(Queue, 0, &err);	//等待消息
if(err == OS_NO_ERR)	//正常		接收到的消息队列状态
{
	Y = *((u8*)pMsg);	
}

u8 X=99;			//消息要携带的数据
if(Queue != NULL) 	//如果创建了消息队列信号量
	OSQPost(Queue, (void*)&X);	//发送消息队列
*/


/*内存分配
#define OS_MEM_EN                 1 		//os_cfg.h 打开ucosii内存分配功能
OS_MEM *Mem;								//定义内存分配控制器
u8 x[4]={0};								//定义内存空间
Mem = OSMemCreate(x, 3, sizeof(x), &err);	//将内存空间和内存分配器捆绑
u8* p;										//用户指针
u8 err;										//用户申请内存空间返回错误值
p = OSMemGet(Mem, &err);					//用户申请内存空间
OSMemPut(Mem, p);							//用户释放内存空间
*/



