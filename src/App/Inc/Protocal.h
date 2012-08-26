#ifndef __PROTOCAL_H
#define __PROTOCAL_H

typedef enum {
	CMD_READ_INT1=0,
	CMD_READ_INT2,
	CMD_READ_INT3,
	CMD_READ_INT4,
	CMD_READ_FLOAT1,
	CMD_READ_FLOAT2,
	CMD_READ_FLOAT3,
	CMD_READ_FLOAT4,
	CMD_READ_DATE,
    	CMD_READ_TIME ,		//设备的时间 	
	CMD_LOCAL_IP,		
	CMD_LOCAL_PORT,
	CMD_GATEWAY,
	CMD_NETMASK,
	CMD_MAC,
	CMD_DEV_ADDR ,		//设备的地址
	CMD_CPU_MODE,	//主从秤台
	CMD_REPORT_STATE, //报告设备状态
	CMD_RESET_CNT, //系统复位次数
	CMD_MEM_DEBUG, //动态内存调试信息，显示当前动态内存使用量
	CMD_NET_SEND_AD, //通过网络发送波形
	CMD_UDP_IP,
	CMD_UDP_PORT,
	CMD_MAX,
}TCmdType;





#endif

