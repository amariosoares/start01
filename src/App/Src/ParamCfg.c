//---------------------------------------------------------------------------


#include "includes.h"
#include "ParamCfg.h"

//---------------------------------------------------------------------------


static struct Param_Device *g_curTarget = NULL;

struct Param_Device* Param_dev_Request(const char* name)
{
	 struct Param_Device * cur = NULL;
	 cur = g_curTarget;
	while(cur){
		if(strncmp(cur->name,name,MAX_PAR_DEVICE_NAME)==0){
			break;
		}
		cur = cur->next;
	}
	return cur;
}
BOOL	Param_Register_Device(struct Param_Device* device)
{
	if(device==NULL)return FALSE;
	
	 device->next 		= g_curTarget;
	g_curTarget 		= device;
	
	if(g_curTarget && g_curTarget->Init){
		g_curTarget->Init(device);
	}
	return TRUE;

}
BOOL    Param_WriteInteger(struct Param_Device* dev,int addr, DWORD  value)
{
     if(dev){
         return dev->Write(dev,addr,value);
     }
     return FALSE;
}
DWORD   Param_ReadInteger(struct Param_Device* dev,int addr,  DWORD  value)
{
     if(dev){
         return dev->Read(dev,addr,value);
     }
     return value;
}
BOOL    Param_WriteFloat(struct Param_Device* dev,int addr,   float  value)
{
     if(dev){
         return dev->WriteFloat(dev,addr,value);
     }
     return FALSE;
}
float   Param_ReadFloat(struct Param_Device* dev,int addr,    float  value)
{
     if(dev){
         return dev->ReadFloat(dev,addr,value);
     }
     return value;
}

BOOL	Param_WriteBuffer(struct Param_Device* dev,int addr,  u8* data, u16 size)
{
	 if(dev){
         return dev->WriteBuffer(dev,addr,data,size);
     }
     return FALSE;
}
BOOL	Param_ReadBuffer(struct Param_Device* dev,int  addr,  u8* data, u16 size)
{
	 if(dev){
         return dev->ReadBuffer(dev,addr,data,size);
     }
     return FALSE;
}
int     	Param_Devsize(struct Param_Device* dev)
{
	if(dev){
		return dev->GetSize(dev);
	}
	return 0;
}
