#include "includes.h"
#include "ParamCfg.h"
#include "FM25L16.h"
#if (DEVICE_FM25L16_EN > 0)
#define DEV_NAME "fm25l16"
static struct Param_Device dev;

#define FM25L16B_CS_HIGH 			(GPIOA->BSRR = GPIO_Pin_4)
#define FM25L16B_CS_LOW				(GPIOA->BRR  = GPIO_Pin_4)

//#define FM25L16B_SO_HIGH			(GPIOA->BSRR = GPIO_Pin_14)
//#define FM25L16B_SO_LOW				(GPIOA->BRR	= GPIO_Pin_14)

#define FM25L16B_SI_HIGH			(GPIOA->BSRR = GPIO_Pin_7)
#define FM25L16B_SI_LOW				(GPIOA->BRR  = GPIO_Pin_7)

#define FM25L16B_SCK_HIGH			(GPIOA->BSRR = GPIO_Pin_5)
#define FM25L16B_SCK_LOW			(GPIOA->BRR  = GPIO_Pin_5)

#define FM25L16B_SO_Read			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)

/*********************************************************************************************************
  命令字宏定义 --
*********************************************************************************************************/
#define FM25L16B_CMD_WREN           0x06			/*Set Write Enable Latch*/			
#define FM25L16B_CMD_WRDI           0x04			/*Write Disable*/
#define FM25L16B_CMD_RDSR  			0x05			/*Read Status Register*/
#define FM25L16B_CMD_WRSR			0x01			/*Write Status Register*/
#define	FM25L16B_CMD_READ			0x03			/*Read Memory Data*/
#define FM25L16B_CMD_WRITE			0x02			/*Write Memory Data*/


#define Delay_Nus(n) 	Delay_US(n)

/*********************************************************************************************************
** Function name:       FM25L16B_WriteRead
** Descriptions:        SPIv写8个数据
** input parameters:    cmd:要写入的命令
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
u8 FM25L16B_WriteRead(u8 cmd)
{
	u8 i,Data_in=0;

	FM25L16B_CS_LOW;				/*使能芯片*/
	for(i=0;i<8;i++)
	{
		FM25L16B_SCK_LOW;			/*SCK拉低*/
		/*写入MOSI*/
		((cmd&0x80)==0x80)?	FM25L16B_SI_HIGH : FM25L16B_SI_LOW;
		Delay_Nus(1);				/*延时长度待测*/
		cmd=cmd<<1;
		FM25L16B_SCK_HIGH;		    /*SCK置高*/

		Delay_Nus(1);				/*延时长度待测*/

		/*读MISO*/
		Data_in<<=1;
		Data_in|=FM25L16B_SO_Read;

	}
    Delay_Nus(1);					/*延时长度待测*/
	FM25L16B_SCK_LOW;		    	/*SCK拉低*/

	return Data_in;
}
/*********************************************************************************************************
** Function name:       FM25L16B_WriteSTA
** Descriptions:        SPIv写状态寄存器
** input parameters:    data:要写入的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void FM25L16B_WriteSTA(u8 data)
{
	FM25L16B_CS_LOW;
	
	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*写使能*/
	FM25L16B_WriteRead(FM25L16B_CMD_WRSR);			/*写状态寄存器*/
	FM25L16B_WriteRead(data);						/*要写入的数据*/

	FM25L16B_CS_HIGH;
}
/*********************************************************************************************************
** Function name:       FM25L16B_ReadSTA
** Descriptions:        SPIv读状态寄存器
** input parameters:    无
** output parameters:   无
** Returned value:      读出的值
*********************************************************************************************************/
u8 FM25L16B_ReadSTA(void)
{
	u8 Data;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_RDSR);			/*读状态寄存器命令*/
	Data = FM25L16B_WriteRead(0xff);				/*读数据,暂不确定此时SI是否拉低，风险点1*/

	FM25L16B_CS_HIGH;
	
	return Data;		
}
/*********************************************************************************************************
** Function name:       FM25L16B_ReadData
** Descriptions:        SPIv读数据
** input parameters:    address:要读的地址
** output parameters:   无
** Returned value:      读取的数据
*********************************************************************************************************/
u8 FM25L16B_ReadData(u16 address)
{
	u8 Data=0;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_READ);			/*读数据命令*/
	FM25L16B_WriteRead(address>>8);					/*发送地址高八位*/
	FM25L16B_WriteRead((address&0x00ff));			/*发送地址低八位*/
	Data = FM25L16B_WriteRead(0xff);				/*读数据,暂不确定此时SI是否拉低，风险点1*/

	FM25L16B_CS_HIGH;

	return Data;
}

/*********************************************************************************************************
** Function name:       FM25L16B_ReadData_Len
** Descriptions:        SPIv读数据
** input parameters:    address:要读的地址
						data_array:要存放的数组头
						length:要读取的数据长度
** output parameters:   无
** Returned value:      读取的数据
*********************************************************************************************************/
void FM25L16B_ReadData_Len(u16 address,u8 *data_array,u16 length)
{
	u16 i;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_READ);			/*读数据命令*/
	FM25L16B_WriteRead(address>>8);					/*发送地址高八位*/
	FM25L16B_WriteRead((address&0x00ff));			/*发送地址低八位*/
	for(i=0;i<length;i++)
	{	
		*(data_array+i) = FM25L16B_WriteRead(0xff);	/*读数据,暂不确定此时SI是否拉低，风险点1*/
	}
	FM25L16B_CS_HIGH;
}
/*********************************************************************************************************
** Function name:       FM25L16B_WriteData
** Descriptions:        SPIv写8个数据
** input parameters:    address: 要写入数据的地址
						data：   写入的数据
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void FM25L16B_WriteData(u16 address,u8 data)
{
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*写使能*/
    FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRITE);			/*写数据命令*/
	FM25L16B_WriteRead(address>>8);					/*发送地址高八位*/
	FM25L16B_WriteRead((address&0x00ff));			/*发送地址低八位*/
	FM25L16B_WriteRead(data);						/*写入数据*/
	FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRDI);			/*写禁止*/
	FM25L16B_CS_HIGH;	
}

/*********************************************************************************************************
** Function name:       FM25L16B_WriteData_Len
** Descriptions:        SPIv写8个数据
** input parameters:    address: 要写入数据的地址
						data_array：   写入的数组
						length:	 长度
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void FM25L16B_WriteData_Len(u16 address,u8 *data_array,u16 length)
{
	u16 i;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*写使能*/
    FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRITE);			/*写数据命令*/
	FM25L16B_WriteRead(address>>8);					/*发送地址高八位*/
	FM25L16B_WriteRead((address&0x00ff));			/*发送地址低八位*/

	for(i=0;i<length;i++)
	{
		FM25L16B_WriteRead(*(data_array+i));		/*写入数据*/
	}
	FM25L16B_CS_HIGH;
	
	FM25L16B_WriteRead(FM25L16B_CMD_WRDI);			/*写禁止*/
	FM25L16B_CS_HIGH;	
}

BOOL    FM25L16_Param_Init(struct Param_Device* dev)
{
	return TRUE;
}
BOOL    FM25L16_Param_Write(struct Param_Device* dev,int addr,  DWORD  value)
{
	FM25L16B_WriteData_Len(addr, (u8*)&value, sizeof(DWORD));
	return TRUE;
}
DWORD   FM25L16_Param_Read(struct Param_Device* dev,int  addr,  DWORD  value)
{
	FM25L16B_ReadData_Len(addr, (u8*)&value, sizeof(DWORD));
	return value;
}
BOOL    FM25L16_WriteFloat(struct Param_Device* dev,int addr,   float  value)
{
	FM25L16B_WriteData_Len(addr, (u8*)&value, sizeof(float));
	return TRUE;
}
float   FM25L16_ReadFloat(struct Param_Device* dev,int addr,    float  value)
{
	FM25L16B_ReadData_Len(addr, (u8*)&value , sizeof(float));
	return value;
}
BOOL	FM25L16_WriteBuffer(struct Param_Device* dev,int addr,  u8* data, u16 size)
{
	FM25L16B_WriteData_Len(addr, data, size);
	return TRUE;
}
BOOL	FM25L16_ReadBuffer(struct Param_Device* dev,int  addr,  u8* data, u16 size)
{
	 FM25L16B_ReadData_Len(addr, data, size);
	 return TRUE;
}
void fm25l6_hard_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;						/* 定义GPIO结构体 */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//为CS5532 引脚分配时钟
			
	/*
	 * FM25L16B管脚定义
	 *	  SPI2_SCK_A :	PD11		Output
	 *	  SPI2_MOSI_A : PB15		Output
	 *	  SPI2_MISO_A : PB14		Input
	 *	  SPI2_CS1#_A : PD13		Output
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_7 ;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			/* 设置响应速度为50M */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			/* 设置引脚为推挽输出 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			/* 设置响应速度为50M */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				/* 设置引脚为下拉输入 */	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
}
u32 FM25L16_GetSize(struct Param_Device* dev)
{
	return 2048;
}

int FM25L16_Init(void)
{
	memset(&dev, 0 , sizeof(struct Param_Device));
	
	strncpy(dev.name,DEV_NAME,MAX_PAR_DEVICE_NAME);

	fm25l6_hard_init();

	dev.Init	   = FM25L16_Param_Init;
	dev.Write 	   = FM25L16_Param_Write;
	dev.Read  	   = FM25L16_Param_Read;
	dev.ReadFloat  = FM25L16_ReadFloat;
	dev.WriteFloat = FM25L16_WriteFloat;
	dev.ReadBuffer = FM25L16_ReadBuffer;
	dev.WriteBuffer= FM25L16_WriteBuffer;
	dev.GetSize	= FM25L16_GetSize;
	return Param_Register_Device(&dev);
}
device_initcall(FM25L16_Init);
#endif
