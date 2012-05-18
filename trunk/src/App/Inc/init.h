#ifndef _INIT_H
#define _INIT_H
typedef int (*initcall_t)(void);
#define __define_initcall(level,fn,id) \
	static initcall_t __initcall_##fn##id __attribute__((__used__)) \
	__attribute__((__section__("initcall" level))) = fn
	
#define core_initcall(fn)		__define_initcall("1",fn,1)	
#define coredevice_initcall(fn)		__define_initcall("2",fn,2)
#define device_initcall(fn)		__define_initcall("3",fn,2)
#define module_initcall(fn)		__define_initcall("4",fn,3)

#define DEBUG_FUNC() DebugPf("call %s\r\n",__FUNCTION__)
#endif

