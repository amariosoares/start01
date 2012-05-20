#include "static_init.h"

u8 test=0;					//测试下断点用
/****************************************************************************
* 名	称：void Delay(u32 nCount)
* 功	能：一个工作的频率延时
* 入口参数：u32 nCount	延时量
* 出口参数：无
* 说	明：无
****************************************************************************/
void Delay(u32 nCount)
{
  while(nCount--);
}

/****************************************************************************
* 名	称：void Delay_US(u32 nCount_temp)
* 功	能：微秒级
* 入口参数：u32 nCount_temp	延时量
* 出口参数：无
* 说	明：无
****************************************************************************/
void Delay_US(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8;
	while(nCount--);
}

/****************************************************************************
* 名	称：void Delay_MS(u32 nCount_temp)
* 功	能：毫秒级
* 入口参数：u32 nCount_temp	延时量
* 出口参数：无
* 说	明：无
****************************************************************************/
void Delay_MS(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000;
	while(nCount--);
}

/****************************************************************************
* 名	称：void Delay_S(u32 nCount_temp)
* 功	能：秒级
* 入口参数：u32 nCount_temp	延时量
* 出口参数：无
* 说	明：无
****************************************************************************/
void Delay_S(u32 nCount_temp)
{
	u32 nCount=nCount_temp*8000000;
	while(nCount--);
}

/****************************************************************************
* 名	称：__asm void NOPDelay(void)
* 功	能：NOP指令周期延迟	   (差不多一个延迟1微秒)
* 入口参数：无
* 出口参数：无
* 说	明：调用一次延时一个系统时钟周期	  
****************************************************************************/
__asm void NOPDelay(void)	  //NOP指令周期级延迟
{
	NOP;
}

/****************************************************************************
* 名	称：u32 Get_SyS_CLK(u8 temp)
* 功	能：当前系统各个频率状态获取	   
* 入口参数：temp 选择频率参数
* 出口参数：时钟频率
* 说	明：分频频率(0) HCLK频率(1) PCLK1频率(2) PCLK2频率(3) SYSTEM频率(其它)
			x=Get_SyS_CLK(1); //分频频率
			printf("ADCCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(2); //HCLK频率
			printf("HCLK=%ldMHz\r\n",x);
			x=Get_SyS_CLK(3); //PCLK1频率
			printf("PCLK1=%ldMHz\r\n",x);
			x=Get_SyS_CLK(4); //PCLK2频率
			printf("PCLK2=%ldMHz\r\n",x);
			x=Get_SyS_CLK(5); //SYSTEM频率 //其它
			printf("SYSCLK=%ldMHz\r\n",x);			
****************************************************************************/
u32 Get_SyS_CLK(u8 temp)
{
	RCC_ClocksTypeDef RCC_ClockFreq;
	RCC_GetClocksFreq(&RCC_ClockFreq);	//为时钟变量赋予片上各时钟
	switch (temp)
	{
		case 1:	return RCC_ClockFreq.ADCCLK_Frequency; 	//分频频率
		case 2:	return RCC_ClockFreq.HCLK_Frequency; 	//HCLK频率
		case 3:	return RCC_ClockFreq.PCLK1_Frequency; 	//PCLK1频率
		case 4:	return RCC_ClockFreq.PCLK2_Frequency; 	//PCLK2频率
		default: return RCC_ClockFreq.SYSCLK_Frequency;	//SYSTEM频率
	}	
}


/****************************************************************************
* 名	称：u8 AsciiToHex(char ch)
* 功	能：单个ASCII码转16进制		   
* 入口参数：ch	'A'-'F'或'0'-'9'
* 出口参数：转换后的0x0A-0x0F.0x00-0x09
* 说	明：ASCII码A-F.0-9转16进制0x0A-0x0F.0x00-0x09
****************************************************************************/
u8 AsciiToHex(char ch)
{
	if(ch >= '0' && ch <= '9')	//数字0~9
	{
		return (ch - 0x30);
	}
	else
	{
		if(ch >= 'A' && ch <= 'F')	//字母A~F
		{
			return (ch - 0x37);
		}
		else
		{
			return 0xFF;
		}
	}
}


/****************************************************************************
* 名	称：void GetCpuIdHex(void)
* 功	能：获取CPU芯片ID值16进制   
* 入口参数：无
* 出口参数：无
* 说	明：6个u16的值存放CPUID
			u16 x[6]={0};
****************************************************************************/
void GetCpuIdHex(u8* HCpuId)
{
	u32 CpuId[3]={0};

	CpuId[0] = (*(u32*)(0x1FFFF7E8));
	CpuId[1] = (*(u32*)(0x1FFFF7EC));
	CpuId[2] = (*(u32*)(0x1FFFF7F0));
	
	HCpuId[0] = (u8)(CpuId[0]>>24);
	HCpuId[1] = (u8)(CpuId[0]>>16); 
	HCpuId[2] = (u8)(CpuId[0]>>8);
	HCpuId[3] = (u8)(CpuId[0]);

	HCpuId[4] = (u8)(CpuId[1]>>24);
	HCpuId[5] = (u8)(CpuId[1]>>16); 
	HCpuId[6] = (u8)(CpuId[1]>>8);
	HCpuId[7] = (u8)(CpuId[1]);

	HCpuId[8] = (u8)(CpuId[2]>>24);
	HCpuId[9] = (u8)(CpuId[2]>>16); 
	HCpuId[10] = (u8)(CpuId[2]>>8);
	HCpuId[11] = (u8)(CpuId[2]);
}

/****************************************************************************
* 名	称：u8 JZ_chack(void)
* 功	能：8M主晶振起振检测 
* 入口参数：无
* 出口参数：u8	1起振	0没起振
* 说    明：OSC_IN(5,12,23)	
			OSC_OUT(6,13,24)
* 测试代码			
	flag=JZ_chack();
	if(flag==1)		//起振
		flag=9;
	else			//没起振
		flag=9;
****************************************************************************/
u8 JZ_chack(void)
{
	u8 flag=0;		 //晶振起振检测延时

	RCC_DeInit();			 		//将外设 RCC寄存器重设为缺省值
	RCC_HSEConfig(RCC_HSE_ON);		//设置外部8M高速晶振(HSE)
									//RCC_HSE_OFF  		HSE 	晶振 OFF 
									//RCC_HSE_ON  		HSE 	晶振 ON 
									//RCC_HSE_Bypass  	HSE 	晶振被外部时钟旁路
	flag = RCC_WaitForHSEStartUp();	//等待 HSE 起振，该函数将等待直到 HSE 就绪，或者在超时的情况下退出 
	if(flag == SUCCESS)
		return 1;
	else
		return 0;
}

/****************************************************************************
* 名	称：void RCC_USEHSI(void)
* 功	能：启用STM32内部RC晶振
* 入口参数：无
* 出口参数：无
* 说	明：
		如果使用内部RC振荡器而不使用外部晶振，请按照下面方法处理：
		1 对于100脚或144脚的产品，OSC_IN应接地，OSC_OUT应悬空。
		2 对于少于100脚的产品，有2种接法：
			2.1 OSC_IN和OSC_OUT分别通过10K电阻接地。此方法可提高EMC性能。
			2.2 分别重映射OSC_IN和OSC_OUT至PD0和PD1，再配置PD0和PD1为推挽输出并输出'0'。此方法可以减小功耗并(相对上面2.1)节省2个外部电阻。
* 测    试：
	u32 x=0;
	while(1)
	{
		printf("--- %d ---\r\n",x);
		x++;
		Delay_S(1);
	}
****************************************************************************/

/****************************************************************************
* 名	称：void System_Reset(void)
* 功	能：系统热重启
* 入口参数：无
* 出口参数：无
* 说	明：
		__enable_irq()  //开中断
		__disable_irq() //关中断 
* 测    试：
//重启方式1：
	u8 x=0;
	printf("START\r\n");
	while(1)
	{
		printf("%d\r\n",x++);
		if(x==5)				//数到10重启
		{
			System_Reset();
			while(1){;}			//如果复位失败则死在这里
		}
		Delay_S(1);
	}
	//__asm	汇编指令

//重启方式2：	
	//NVIC_SystemReset();	//固件库自带重启命令
	u8 x=0;
	printf("START\r\n");
	while(1)
	{
		printf("%d\r\n",x++);
		if(x==5)				//数到10重启
		{
			__disable_irq();	//禁止一切中断产生 
			NVIC_SystemReset();
			while(1){;}			//如果复位失败则死在这里
		}
		Delay_S(1);
	}
****************************************************************************/
__asm void System_Reset(void) 
{ 
	MOV R0, #1           //  
	MSR FAULTMASK, R0    // 清除FAULTMASK 禁止一切中断产生 
	LDR R0, =0xE000ED0C  // 
	LDR R1, =0x05FA0004  //  
	STR R1, [R0]         // 系统软件复位    
}

