//---------------------------------------------------------------------------

#ifndef ParamCfgH
#define ParamCfgH

#define DEV_FM25L16 "fm25l16"
#define DEV_BPK             "stm32_bpk"

typedef unsigned int DWORD;
#define MAX_PAR_DEVICE_NAME  16
struct Param_Device{
	char	name[MAX_PAR_DEVICE_NAME];
    BOOL  	  (*Init)(struct Param_Device* dev);
    u32	 (*GetSize)(struct Param_Device* dev);
    BOOL    	  (*Write)(struct Param_Device* dev,int addr,  DWORD  value);
    DWORD   (*Read)(struct Param_Device* dev,int  addr,  DWORD  value);
    BOOL    	  (*WriteFloat)(struct Param_Device* dev,int addr,   float  value);
    float  	  (*ReadFloat)(struct Param_Device* dev,int addr,    float  value);
	BOOL	(*WriteBuffer)(struct Param_Device* dev,int addr,  u8* data, u16 size);
	BOOL	(*ReadBuffer)(struct Param_Device* dev,int  addr,  u8* data, u16 size);
   struct Param_Device* next;
};

struct Param_Device* Param_dev_Request(const char* name);
BOOL    	Param_WriteInteger(struct Param_Device* dev,int addr, DWORD  value);
DWORD    Param_ReadInteger(struct Param_Device* dev,int addr,  DWORD  value);
BOOL    	Param_WriteFloat(struct Param_Device* dev,int addr,   float  value);
float   	Param_ReadFloat(struct Param_Device* dev,int addr,    float  value);
BOOL	Param_WriteBuffer(struct Param_Device* dev,int addr,  u8* data, u16 size);
BOOL	Param_ReadBuffer(struct Param_Device* dev,int  addr,  u8* data, u16 size);
int     	Param_Devsize(struct Param_Device* dev);

BOOL	Param_Register_Device(struct Param_Device* device);

#define GET_OFFSET(addr) (addr*sizeof(DWORD))
/*
U16  Param_ReadU16(eParamTypes key, U16 value);
U32  Param_ReadU32(eParamTypes key, U32 value);
*/
//---------------------------------------------------------------------------
#endif
