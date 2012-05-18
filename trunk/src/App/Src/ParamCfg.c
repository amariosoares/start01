//---------------------------------------------------------------------------


#include "includes.h"
#include "ParamCfg.h"

//---------------------------------------------------------------------------


static Param_Device *g_curTarget = NULL;

BOOL	Param_Register_Device(Param_Device* device)
{
	g_curTarget = device;
	if(g_curTarget && g_curTarget->Init){
		g_curTarget->Init();
	}
	return TRUE;

}
BOOL    Param_WriteInteger(int addr, DWORD  value)
{
     if(g_curTarget){
         return g_curTarget->Write(addr,value);
     }
     return FALSE;
}
DWORD   Param_ReadInteger(int addr,  DWORD  value)
{
     if(g_curTarget){
         return g_curTarget->Read(addr,value);
     }
     return value;
}
BOOL    Param_WriteFloat(int addr,   float  value)
{
     if(g_curTarget){
         return g_curTarget->WriteFloat(addr,value);
     }
     return FALSE;
}
float   Param_ReadFloat(int addr,    float  value)
{
     if(g_curTarget){
         return g_curTarget->ReadFloat(addr,value);
     }
     return value;
}

BOOL	Param_WriteBuffer(int addr,  u8* data, u16 size)
{
	 if(g_curTarget){
         return g_curTarget->WriteBuffer(addr,data,size);
     }
     return FALSE;
}
BOOL	Param_ReadBuffer(int  addr,  u8* data, u16 size)
{
	 if(g_curTarget){
         return g_curTarget->ReadBuffer(addr,data,size);
     }
     return FALSE;
}

