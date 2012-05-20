#ifndef __DEBUG_COMM_H
#define __DEBUG_COMM_H

#include "Protocal.h"

//#define LITTLE_ENDIAN 1
//#define BIG_ENDIAN	  2

//#define BYTE_ENDIAN	  BIG_ENDIAN
#ifndef UPDATE_PARAM_EN
#define UPDATE_PARAM_EN 1
#define PARAM_DEV_NAME "fm25l16"
#endif
#define MAX_CMD_NUM  128
#define MAX_FUNC_NUM 1

typedef enum{
	DIR_CONTRL,
	DIR_READ,
	DIR_WRITE,
	DIR_ACK,
	MAX_DIR_SIZE
}TCmdDir;
#if 0
#define DIR_CONTRL   0
#define DIR_READ     1
#define DIR_WRITE    2
#define DIR_ACK      3
#define MAX_DIR_SIZE 	3
#endif



#if 1
#define PACK __packed
#else
#define PACK
#endif


PACK typedef  union{	
		u8	  u8_val;
		u16   u16_val;
		u32	  u32_val;
		float f32_val;
		PACK struct{
			u8 pad[4];
		}__pad;
		PACK struct{
			u8 d4;
			u8 d3;
			u8 d2;
			u8 d1;
		}buf;
}TVarType;

PACK typedef struct {
	u8  head1;
	u8  head2;
	u16 dev_id;
	u8  dir;
	u8  cmd;
	TVarType param;
	u8 check;
}TCommMsg;


typedef enum tag_DataChan{
	CHAN_NULL = 0,	//discard data
	CHAN_USART1 , //debug display  	 m/s
	CHAN_USART2, //MCU intercomm 	 m/s
	CHAN_USART3, //Ô¤Áô[rs232]	 m
	CHAN_USART4, //PC  [rs232/rs485]   m
	CHAN_USART5, //Ô¤Áô[rs485] 	m/s
	CHAN_ETH0,
}TDataChan;

#define MAX_DEBUG_MSG		10
#define RECV_BUF_SIZE  		sizeof(TCommMsg)*MAX_DEBUG_MSG

typedef u8 (*cmd_func)(TDataChan *src, TCommMsg* msg);
typedef u32	(*ack_func)(TDataChan dest,u8* data, u32 len);
typedef u8 (*route_table_func)(u8 src,TCommMsg *msg);
struct TCmd{
	u8 dir;
	u8 cmd;
	cmd_func func;
	struct TCmd* next;
};

#define CMD_ITEM(_dir,_cmd,_func) {.dir=_dir,.cmd=_cmd,.func=_func,.next=NULL}

#define VAR_UPDATE_MEMORY 1
#ifdef UPDATE_PARAM_EN
#define VAR_UPDATE_FLASH  2
#endif
typedef struct {
	u8 		cmd;
	u8 		var_size;
	void* 	var_addr;
	u8		var_type;
	u8		reg_flag;
}TVarItem;


extern void   ParseDebugCommMsg(TDataChan src,u8* data, u32 len,ack_func ack);
extern u8 	  RegisterCmd(struct TCmd* cmd);
extern u8     RegisterCmdList(struct TCmd* cmd_list,u8 cmd_size);
extern u8 	  RegisterAutoVar(u8 cmd,u8 var_size,void* var_addr,u8 var_type);
extern u8     RegisterAutoVarList(TVarItem* var_list, u8 var_size);
extern u8	  RegisterRouteTable(route_table_func func);
extern u8	  BuildDebugMsgBuf(TCommMsg* msg, u8* buf, u8 len);
#endif

