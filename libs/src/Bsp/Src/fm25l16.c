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
  �����ֺ궨�� --
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
** Descriptions:        SPIvд8������
** input parameters:    cmd:Ҫд�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
u8 FM25L16B_WriteRead(u8 cmd)
{
	u8 i,Data_in=0;

	FM25L16B_CS_LOW;				/*ʹ��оƬ*/
	for(i=0;i<8;i++)
	{
		FM25L16B_SCK_LOW;			/*SCK����*/
		/*д��MOSI*/
		((cmd&0x80)==0x80)?	FM25L16B_SI_HIGH : FM25L16B_SI_LOW;
		Delay_Nus(1);				/*��ʱ���ȴ���*/
		cmd=cmd<<1;
		FM25L16B_SCK_HIGH;		    /*SCK�ø�*/

		Delay_Nus(1);				/*��ʱ���ȴ���*/

		/*��MISO*/
		Data_in<<=1;
		Data_in|=FM25L16B_SO_Read;

	}
    Delay_Nus(1);					/*��ʱ���ȴ���*/
	FM25L16B_SCK_LOW;		    	/*SCK����*/

	return Data_in;
}
/*********************************************************************************************************
** Function name:       FM25L16B_WriteSTA
** Descriptions:        SPIvд״̬�Ĵ���
** input parameters:    data:Ҫд�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void FM25L16B_WriteSTA(u8 data)
{
	FM25L16B_CS_LOW;
	
	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*дʹ��*/
	FM25L16B_WriteRead(FM25L16B_CMD_WRSR);			/*д״̬�Ĵ���*/
	FM25L16B_WriteRead(data);						/*Ҫд�������*/

	FM25L16B_CS_HIGH;
}
/*********************************************************************************************************
** Function name:       FM25L16B_ReadSTA
** Descriptions:        SPIv��״̬�Ĵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:      ������ֵ
*********************************************************************************************************/
u8 FM25L16B_ReadSTA(void)
{
	u8 Data;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_RDSR);			/*��״̬�Ĵ�������*/
	Data = FM25L16B_WriteRead(0xff);				/*������,�ݲ�ȷ����ʱSI�Ƿ����ͣ����յ�1*/

	FM25L16B_CS_HIGH;
	
	return Data;		
}
/*********************************************************************************************************
** Function name:       FM25L16B_ReadData
** Descriptions:        SPIv������
** input parameters:    address:Ҫ���ĵ�ַ
** output parameters:   ��
** Returned value:      ��ȡ������
*********************************************************************************************************/
u8 FM25L16B_ReadData(u16 address)
{
	u8 Data=0;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_READ);			/*����������*/
	FM25L16B_WriteRead(address>>8);					/*���͵�ַ�߰�λ*/
	FM25L16B_WriteRead((address&0x00ff));			/*���͵�ַ�Ͱ�λ*/
	Data = FM25L16B_WriteRead(0xff);				/*������,�ݲ�ȷ����ʱSI�Ƿ����ͣ����յ�1*/

	FM25L16B_CS_HIGH;

	return Data;
}

/*********************************************************************************************************
** Function name:       FM25L16B_ReadData_Len
** Descriptions:        SPIv������
** input parameters:    address:Ҫ���ĵ�ַ
						data_array:Ҫ��ŵ�����ͷ
						length:Ҫ��ȡ�����ݳ���
** output parameters:   ��
** Returned value:      ��ȡ������
*********************************************************************************************************/
void FM25L16B_ReadData_Len(u16 address,u8 *data_array,u16 length)
{
	u16 i;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_READ);			/*����������*/
	FM25L16B_WriteRead(address>>8);					/*���͵�ַ�߰�λ*/
	FM25L16B_WriteRead((address&0x00ff));			/*���͵�ַ�Ͱ�λ*/
	for(i=0;i<length;i++)
	{	
		*(data_array+i) = FM25L16B_WriteRead(0xff);	/*������,�ݲ�ȷ����ʱSI�Ƿ����ͣ����յ�1*/
	}
	FM25L16B_CS_HIGH;
}
/*********************************************************************************************************
** Function name:       FM25L16B_WriteData
** Descriptions:        SPIvд8������
** input parameters:    address: Ҫд�����ݵĵ�ַ
						data��   д�������
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void FM25L16B_WriteData(u16 address,u8 data)
{
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*дʹ��*/
    FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRITE);			/*д��������*/
	FM25L16B_WriteRead(address>>8);					/*���͵�ַ�߰�λ*/
	FM25L16B_WriteRead((address&0x00ff));			/*���͵�ַ�Ͱ�λ*/
	FM25L16B_WriteRead(data);						/*д������*/
	FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRDI);			/*д��ֹ*/
	FM25L16B_CS_HIGH;	
}

/*********************************************************************************************************
** Function name:       FM25L16B_WriteData_Len
** Descriptions:        SPIvд8������
** input parameters:    address: Ҫд�����ݵĵ�ַ
						data_array��   д�������
						length:	 ����
** output parameters:   ��
** Returned value:      ��
*********************************************************************************************************/
void FM25L16B_WriteData_Len(u16 address,u8 *data_array,u16 length)
{
	u16 i;
	FM25L16B_CS_LOW;

	FM25L16B_WriteRead(FM25L16B_CMD_WREN);			/*дʹ��*/
    FM25L16B_CS_HIGH;

	FM25L16B_WriteRead(FM25L16B_CMD_WRITE);			/*д��������*/
	FM25L16B_WriteRead(address>>8);					/*���͵�ַ�߰�λ*/
	FM25L16B_WriteRead((address&0x00ff));			/*���͵�ַ�Ͱ�λ*/

	for(i=0;i<length;i++)
	{
		FM25L16B_WriteRead(*(data_array+i));		/*д������*/
	}
	FM25L16B_CS_HIGH;
	
	FM25L16B_WriteRead(FM25L16B_CMD_WRDI);			/*д��ֹ*/
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
	
	GPIO_InitTypeDef GPIO_InitStructure;						/* ����GPIO�ṹ�� */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//ΪCS5532 ���ŷ���ʱ��
			
	/*
	 * FM25L16B�ܽŶ���
	 *	  SPI2_SCK_A :	PD11		Output
	 *	  SPI2_MOSI_A : PB15		Output
	 *	  SPI2_MISO_A : PB14		Input
	 *	  SPI2_CS1#_A : PD13		Output
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 | GPIO_Pin_7 ;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			/* ������Ӧ�ٶ�Ϊ50M */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			/* ��������Ϊ������� */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			/* ������Ӧ�ٶ�Ϊ50M */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				/* ��������Ϊ�������� */	
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
