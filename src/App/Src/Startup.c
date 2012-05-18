#include "includes.h"
#include "Startup.h"

extern unsigned  int Image$$INIT_SECT$$Base;
extern unsigned  int Image$$INIT_SECT$$Length;
extern unsigned  int Image$$INIT_SECT$$Limit;

void InitCall(void)
{
	int base = (unsigned int)&Image$$INIT_SECT$$Base;
	int size = (unsigned int)&Image$$INIT_SECT$$Length;
	int end  = (unsigned int)&Image$$INIT_SECT$$Limit;
	initcall_t *initcall;
	int result;

	//DebugPf("base=0x%x,size=0x%x,end=0x%x\r\n",base,size,end);

	for (initcall = (initcall_t*)base;
			initcall < (initcall_t*)end; initcall++) {

		result = (*initcall)();
		if(!result){
			DebugPf("call 0x%x failed\r\n",*initcall);
		}
	}
	
}

u8 InitSystem(void)
{
	InitCall();
	return 1;
}

