#include "includes.h"
#include "serial.h"
void assert_failed(uint8_t* file, uint32_t line)
{
	DebugPf("assert failed : file %s line %d\r\n",(char*)file, line);
}

