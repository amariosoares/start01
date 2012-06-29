#ifndef _AT45DB321D_INIT_H_
#define _AT45DB321D_INIT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"   //��ʱ

#define MAX_PAGE_NUM     8190
#define MAX_PHY_PAGE_NUM 8191
#define TOTAL_BYTES_PER_PAGE    528
#define AVALID_BYTES_PER_PAGE   512
#define AVALID_PAGE_OFFSET      0
#define BYTES_PER_BLCOK  4224


void AT45DB321D_init(void);							//��ʼ��оƬ����
void AT45DB321D_GetID(u8* DeviceID,u8 Len,u8 num);	//��ȡоƬID��
u8 AT45DB321D_Isbusy(u8 num);						//æ��ѯ
void AT45DB321D_busytime(u8 num);					//æ�ӳ�
void AT45DB321D_Erase(u8 num);						//����оƬ����
void AT45DB321D_CLEAR_PAGE(u16 Page,u8 num);			//ҳ����
void AT45DB321D_BLOCK_PAGE(u16 Block,u8 num);		//����� ÿ8ҳһ����

void AT45DB321D_WritePage(u16 Page,u16 Addr,u8 Buffer,u8* WriteData,u16 Len,u8 num);	 //ҳд
void AT45DB321D_ReadPage(u16 Page,u16 Addr,u8 Buffer,u8* ReadData,u16 Len,u8 num);	 //ҳ��

u8 DiskWrite(u32 Addr, u8* Buffer, u32 Len, u8 Num);	//�ֽڴ�
u8 DiskRead(u32 Addr, u8* Buffer, u32 Len, u8 Num);		//�ֽڶ�

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

#endif
