#include "includes.h"
#include "paramcfg.h"

#define DEV_NAME "stm32_bpk"
#define max_size  42
static BOOL bpk_Param_Init(struct Param_Device* dev)
{
	return TRUE;
}
static BOOL bpk_check_addr(int addr)
{
	return (    (addr >=0) && (addr < max_size)   );
}
//#define BKP_DR1	   ((uint16_t)0x0004)
//#define BKP_DR11							((uint16_t)0x0040)

static u16 bpk_convert_addr(int addr)
{
	addr++;
	if(addr < 11){
		return (u16)(addr<<2);
	}else{
		return (u16)( (addr<<2)+0x14);
	}
}
static BOOL bpk_Param_Write(struct Param_Device* dev, int addr ,DWORD value)
{
	if(dev && (bpk_check_addr(addr))){
		
		BKP_WriteBackupRegister(bpk_convert_addr(addr),value&0xffff);
		return TRUE;
	}

	return FALSE;
}

static DWORD	bpk_Param_Read(struct Param_Device* dev,int  addr,  DWORD	value)
{
	if(dev && (bpk_check_addr(addr))){
		
		value = BKP_ReadBackupRegister(bpk_convert_addr(addr));
	}

	return value;
}
static BOOL	bpk_Param_WriteFloat(struct Param_Device* dev,int addr,	 float	value)
{
	return FALSE;
}
static float	 bpk_Param_ReadFloat(struct Param_Device* dev,int addr,	 float	value)
{
	return value;
}
static BOOL	bpk_Param_WriteBuffer(struct Param_Device* dev,int addr,  u8* data, u16 size)
{
	return FALSE;
}
static BOOL	bpk_Param_ReadBuffer(struct Param_Device* dev,int	addr,  u8* data, u16 size)
{
	return FALSE;
}

static u32 bpk_GetSize(struct Param_Device* dev)
{
	return max_size;
}
static struct Param_Device bpk_dev = {
	.Init	   	 = bpk_Param_Init,
	.Write 	   = bpk_Param_Write,
	.Read  	   = bpk_Param_Read,
	.ReadFloat  = bpk_Param_ReadFloat,
	.WriteFloat = bpk_Param_WriteFloat,
	.ReadBuffer = bpk_Param_ReadBuffer,
	.WriteBuffer= bpk_Param_WriteBuffer,
	.GetSize	    = bpk_GetSize,
};
static int stm32_bpk_init(void)
{
	memset(	bpk_dev.name,0,MAX_PAR_DEVICE_NAME);
	strncpy(bpk_dev.name,DEV_NAME,MAX_PAR_DEVICE_NAME);

	return Param_Register_Device(&bpk_dev);
}

device_initcall(stm32_bpk_init);
