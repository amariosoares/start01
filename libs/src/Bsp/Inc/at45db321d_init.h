#ifndef _AT45DB321D_INIT_H_
#define _AT45DB321D_INIT_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

#include "stm32f10x.h"
#include "static_init.h"   //延时

#define MAX_PAGE_NUM     8190
#define MAX_PHY_PAGE_NUM 8191
#define TOTAL_BYTES_PER_PAGE    528
#define AVALID_BYTES_PER_PAGE   512
#define AVALID_PAGE_OFFSET      0
#define BYTES_PER_BLCOK  4224


void AT45DB321D_init(void);							//初始化芯片引脚
void AT45DB321D_GetID(u8* DeviceID,u8 Len,u8 num);	//获取芯片ID号
u8 AT45DB321D_Isbusy(u8 num);						//忙查询
void AT45DB321D_busytime(u8 num);					//忙延迟
void AT45DB321D_Erase(u8 num);						//整个芯片擦除
void AT45DB321D_CLEAR_PAGE(u16 Page,u8 num);			//页擦除
void AT45DB321D_BLOCK_PAGE(u16 Block,u8 num);		//块擦除 每8页一个块

void AT45DB321D_WritePage(u16 Page,u16 Addr,u8 Buffer,u8* WriteData,u16 Len,u8 num);	 //页写
void AT45DB321D_ReadPage(u16 Page,u16 Addr,u8 Buffer,u8* ReadData,u16 Len,u8 num);	 //页读

u8 DiskWrite(u32 Addr, u8* Buffer, u32 Len, u8 Num);	//字节存
u8 DiskRead(u32 Addr, u8* Buffer, u32 Len, u8 Num);		//字节读

#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
