#ifndef _MAIN_INIT_H_	
#define _MAIN_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "bsp.h"	 		//Ӳ����Դ
#include "includes.h"		//ϵͳ��Դ

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif


/*�ź�������	 //δ�����η����ź��������
OS_EVENT *Signal = 0;		 			//����������ź��� 
if(Signal == NULL) 
	Signal=OSSemCreate(0); 				//����������ź���
	 
u8 Task2xherror=0;		  				//�ȴ�������ź������մ����־

OSSemPend(Task2xh,0,&Task2xherror);		//����ȴ�������ź���
OSSemPost(Task2xh); 					//����������ź��� 
*/



/*�̹߳���
OSTaskSuspend(2);						//�����̹߳���
OSTaskResume(2);						//�����̴߳ӹ����лָ�
OSTaskSuspend(OS_PRIO_SELF);
*/



/*����			 //δ�����η��ͺ󼸴η��ͻ᷵��OS_ERR_MBOX_FULL���󣬵����ʼ�Я����ַ��ı�
#define OS_MBOX_EN                1    	//os_cfg.h ��ucosii���书��

OS_EVENT *MailBox = 0;						//���������ź���
if(MailBox == NULL)
	MailBox= OSMboxCreate((void *)0);		//���������ź���

u8 err;									//������մ����־
u8 z[9]={0};							//�������䴫����ָ����ָ����
void *y;								//���䴫���ĵ�ַָ��

y=OSMboxPend(MailBox, 0, &err);			//������չ���

memcpy(z,(u8*)y,9);						//���յ�����֮��ָ����ָ��ַ�ϵ�����ȡ��
u8 x[9]={1,2,3,4,5,6,7,8,9};			//��Ҫ�����䷢�͵�����

OSMboxPost(MailBox, (void *)&x);		//���ͼ���ȴ�������
*/



/*�����ж�ԭ�Ӳ���
#if OS_CRITICAL_METHOD == 3             //��ʼ��������            
    OS_CPU_SR  cpu_sr;
#endif
OS_ENTER_CRITICAL();					//�����ж�ʧ�ܹ���
OS_EXIT_CRITICAL();						//�ر��ж�ʹ�ܹ���
*/



/*������
#define OS_MUTEX_EN               1     //os_cfg.h ��ucosii����������
OS_EVENT *Mutex = 0;		 			//���廥����

if(Mutex == NULL)
	Mutex = OSMutexCreate(2, &err);		//����������  //�����������ȼ��������ȼ����ܱ�ռ��

OSMutexPend(Mutex, 0, &err);			//��ȡ������

OSMutexPost(Mutex);						//�ͷŻ�����
*/



/*��Ϣ����
#define OS_Q_EN                   1		//os_cfg.h ����Ϣ���й���
#define OS_MAX_QS                10		//os_cfg.h �޸Ķ��п��ƿ�����

#define QueueSize 6						//��Ϣ�������
OS_EVENT *Queue = 0;					//������Ϣ�����ź���
void* QueueBuff[QueueSize];	   			//��Ϣ���л�����

if(Queue == NULL)
	Queue = OSQCreate(QueueBuff, QueueSize);    //������Ϣ�����ź���

void* pMsg;				//��Ϣ������ֵ��ַ
u8 err;					//������Ϣ״̬
u8 Y=0;					//��Ϣ����������
pMsg = OSQPend(Queue, 0, &err);	//�ȴ���Ϣ
if(err == OS_NO_ERR)	//����		���յ�����Ϣ����״̬
{
	Y = *((u8*)pMsg);	
}

u8 X=99;			//��ϢҪЯ��������
if(Queue != NULL) 	//�����������Ϣ�����ź���
	OSQPost(Queue, (void*)&X);	//������Ϣ����
*/


/*�ڴ����
#define OS_MEM_EN                 1 		//os_cfg.h ��ucosii�ڴ���书��
OS_MEM *Mem;								//�����ڴ���������
u8 x[4]={0};								//�����ڴ�ռ�
Mem = OSMemCreate(x, 3, sizeof(x), &err);	//���ڴ�ռ���ڴ����������
u8* p;										//�û�ָ��
u8 err;										//�û������ڴ�ռ䷵�ش���ֵ
p = OSMemGet(Mem, &err);					//�û������ڴ�ռ�
OSMemPut(Mem, p);							//�û��ͷ��ڴ�ռ�
*/



