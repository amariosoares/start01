//---------------------------------------------------------------------------

#ifndef ParamCfgH
#define ParamCfgH
typedef unsigned int DWORD;
#define MAX_PAR_DEVICE_NAME  16
typedef struct tag_Param_Target{
	char	name[MAX_PAR_DEVICE_NAME];
    BOOL    (*Init)(void);
    BOOL    (*Write)(int addr,  DWORD  value);
    DWORD   (*Read)(int  addr,  DWORD  value);
    BOOL    (*WriteFloat)(int addr,   float  value);
    float   (*ReadFloat)(int addr,    float  value);
	BOOL	(*WriteBuffer)(int addr,  u8* data, u16 size);
	BOOL	(*ReadBuffer)(int  addr,  u8* data, u16 size);
}Param_Device;


BOOL    Param_WriteInteger(int addr, DWORD  value);
DWORD   Param_ReadInteger(int addr,  DWORD  value);
BOOL    Param_WriteFloat(int addr,   float  value);
float   Param_ReadFloat(int addr,    float  value);
BOOL	Param_WriteBuffer(int addr,  u8* data, u16 size);
BOOL	Param_ReadBuffer(int  addr,  u8* data, u16 size);

BOOL	Param_Register_Device(Param_Device* device);

#define GET_OFFSET(addr) (addr*sizeof(DWORD))
/*
U16  Param_ReadU16(eParamTypes key, U16 value);
U32  Param_ReadU32(eParamTypes key, U32 value);
*/
//---------------------------------------------------------------------------
#endif
