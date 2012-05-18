#ifndef _STATIC_INIT_H_	
#define _STATIC_INIT_H_

#ifdef __cplusplus		   //定义对CPP进行C处理 //开始部分
extern "C" {
#endif

#include "stm32f10x.h"
#include "String.h"				//memcpy
#include "math.h"				//数学计算
#include "includes.h"			//ucos

#define DebugPf			printf			//调试printf输出
//#define DebugPf

#ifndef BOOL							//BOOL
#define	BOOL			BOOLEAN
#endif

#ifndef 	TRUE							//TRUE
#define		TRUE			1
#endif

#ifndef 	FALSE 							//FALSE
#define		FALSE			0
#endif

#ifndef 	true 							//true
#define		true			1
#endif

#ifndef 	false 							//false
#define		false			0
#endif

//串口状态
#define USARTSTATUS_General		0x00	//通常状态
#define USARTSTATUS_BuffFull	0x01	//缓冲区满 
#define USARTSTATUS_GetKeyChar	0x02	//接收到关键字
#define USARTSTATUS_GetCharLen	0x04	//定长接收
#define USARTSTATUS_Bag			0x08	//包接收

typedef struct
{
	u32 ValidFlag;				//标定个数
	float  WeightVal[10];		//物理值
	float  VotageVal[10];		//模拟值
} CALIBRATION;					//牛顿插值标定

extern u8 test;						//测试下断点用
extern CALIBRATION CalibrationSave;	//牛顿插值标定

void Delay(u32 nCount);				//系统频率级延时
void Delay_US(u32 nCount);			//微秒级延时
void Delay_MS(u32 nCount);			//毫秒级延时
void Delay_S(u32 nCount);			//秒级延时
void NOPDelay(void);					//NOP指令周期延迟
void L_From_B(float* Val,u32 size);	//冒泡由小到大

u32 Get_SyS_CLK(u8 temp);		//当前系统各个频率状态获取	   分频频率(0) HCLK频率(1) PCLK1频率(2) PCLK2频率(3) SYSTEM频率(其它)
u16 StoB_2_BtoS(u16 n);			//12转21
u32 StoB_4_BtoS(u32 n);			//1234转4321 整型
float StoB_F4_BtoS(float f);		//1234转4321 浮点型

u8 AsciiToHex(char ch);				//单个ASCII码A-F.0-9转16进制0x0A-0x0F.0x00-0x09
char HexToAscii(u8 nHex);			//单个16进制0x0A-0x0F.0x00-0x09转ASCII码A-F.0-9
void BufferAsciiToHex(u8 *pAaciiBuf, u8 *pHexBuf, u16 nAsciiLen);	 	//多个ASCII码转16进制	 4位转2位
void BufferHexToAscii(u8 *pHexBuf,u8 *pAaciiBuf , u16 nHexLen);			//多个16进制转ASCII码	 2位转4位

void GetCpuIdHex(u8* CpuId);		//获取CPU芯片ID值16进制 u16 x[6];

u8 NewTon_Num(u16 flag);									//查找当前标定使用个数
void NewTon_Num_init(CALIBRATION* CS,u8 num_Temp);		//牛顿差商计算
float Ca_NewTon(float ADC_temp,CALIBRATION* CS,u8 x);	//牛顿插值计算
long Sample_Wave(float* AD_New,float* AD_Old,long ADOld_Len,float Wave_Temp);	//逐次滤波
u32 ASCIItoNumber(u8* data_temp,u8 len_temp);			//ASCII转数字
u32 BeiNumber(u32 num_temp,u8 len_temp);					//根据输入的位数进行翻倍
u8 JZ_chack(void);  		//8M主晶振起振检测 OSC_IN(5,12,23)	OSC_OUT(6,13,24)
void RCC_USEHSI(void);	//使用STM32内部RC时钟 不精确 慢
void System_Reset(void);	//系统重启
#ifdef __cplusplus		   //定义对CPP进行C处理 //结束部分
}
#endif

#endif
