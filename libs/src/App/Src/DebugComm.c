#include "includes.h"
#include "DebugComm.h"
#include "ParamCfg.h"
#include "protocal.h"
//#define PROT_DEBUG   	1

#define MSG_LEN 	sizeof(TCommMsg)


static  u8	_hasInit = 0;

typedef struct 
{
	struct TCmd* 	   next;
}TCmdActionHead;

static TCmdActionHead cmdlist[MAX_DIR_SIZE][CMD_MAX];

//static TCmdNotifyList  notify_list[MAX_DIR_SIZE];
static TVarItem var_list[MAX_CMD_NUM];
static u8 m_send_buf[sizeof(TCommMsg)];
static route_table_func m_route_func = NULL;

static void Init_CmdList(void);
static void Dump_Msg(TCommMsg* msg);
static void AckCommMsg(TCommMsg * msg,TDataChan dest,ack_func ack);
#ifdef UPDATE_PARAM_EN
static struct Param_Device* dev =  NULL;
#endif
u8 	  RegisterCmd(struct TCmd* item)
{

	u8 dir=0,cmd=0;

	
	if(!item) return 0;
	if(!_hasInit){
		Init_CmdList();
		_hasInit = 1;
	}
	dir = item->dir;
	cmd = item->cmd;
	if( (dir >= MAX_DIR_SIZE) || (cmd >= MAX_CMD_NUM))
		return 0;

	item->next 			    = cmdlist[dir][cmd].next;
	cmdlist[dir][cmd].next = item;

	return 1;
	
}
u8 RegisterCmdList(struct TCmd* cmd_list,u8 cmd_size)
{
	u8 i   = 0;
	u8 ret = 1;
	for(i = 0; i < cmd_size; i++)
	{
		ret	&= RegisterCmd(cmd_list+i);
	}
	
	return ret;
}
static int Var_Auto_Update_Service(TCommMsg* msg)
{
	u8 cmd = msg->cmd;
	u8 dir = msg->dir;
	
	if(cmd >= MAX_CMD_NUM) return 0;
	if( (dir != DIR_READ) && (dir != DIR_WRITE)) 
		return 0;

	if(var_list[cmd].reg_flag == 0) return 0;
	else
	{
		
		u8* p 		= var_list[cmd].var_addr;
		u8  i   	= 0;
		u8 var_size = var_list[cmd].var_size;
		u8 var_type = var_list[cmd].var_type;
		if(p == NULL) 	 return 0;
		if(var_size > 4) return 0;
		
		if(dir == DIR_WRITE)
		{
			if(var_type&VAR_UPDATE_MEMORY){
				for(i = 0; i < var_size; i++)
					p[i] = msg->param.__pad.pad[i];
			}
			#ifdef UPDATE_PARAM_EN
			if(var_type&VAR_UPDATE_FLASH){
				Param_WriteBuffer(dev,GET_OFFSET(cmd),msg->param.__pad.pad,var_size);
			}
			#endif
			
		}
		else if(dir == DIR_READ)
		{
			if(var_type&VAR_UPDATE_MEMORY){
				for(i = 0; i < var_size; i++)
					msg->param.__pad.pad[i] = p[i];
			}
			
		}
	}
	return 1;
}

u8 	  RegisterAutoVar(u8 cmd,u8 var_size,void* var_addr,u8 var_type)
{
	if(cmd >= MAX_CMD_NUM) return 0;

	if(var_size > 4) return 0;

	if(var_addr == NULL) return 0;

	var_list[cmd].cmd  	   = cmd;
	var_list[cmd].var_type = var_type;
	var_list[cmd].reg_flag = 1;
	var_list[cmd].var_size = var_size;
	var_list[cmd].var_addr = var_addr;

	return 1;
}
u8     RegisterAutoVarList(TVarItem* var_list, u8 var_size)
{
	u8 i   = 0;
	u8 ret = 1;

	for(i = 0; i < var_size; i++)
	{
		u8 cmd     		= var_list[i].cmd;
		u8 var_size		= var_list[i].var_size;
		u8 var_type     = var_list[i].var_type;
		void* var_addr	= var_list[i].var_addr;
		
		ret &= RegisterAutoVar(cmd , var_size, var_addr,var_type);
	}

	return ret;
	
}
u8	  RegisterRouteTable(route_table_func func)
{
	m_route_func = func;

	return 1;
}
static int  Cmd_Service(TDataChan src,TCommMsg* msg,ack_func ack)
{
	u8 cmd = msg->cmd;
	u8 dir = msg->dir;
	u8 cnt = 0;
	u8 i   = 0;
	u8 bypass = 0;
	u8 has_cmd = 0;
	//TDataChan tmp_dest = src;
	TDataChan dest     = src;
	#ifdef PROT_DEBUG
		Dump_Msg(msg);
	#endif
	if(!_hasInit) return 0;
	if( (dir >= MAX_DIR_SIZE) || (cmd >= MAX_CMD_NUM))
		return 0;
	
	dir %= MAX_DIR_SIZE;
	cmd %= MAX_CMD_NUM;

	struct TCmd* cmd_head = cmdlist[dir][cmd].next;

	while(cmd_head){ //首先检查是否有注册过的命令，需要人工处理
			
		if(cmd_head->func){
			has_cmd = 1;
			bypass = cmd_head->func(&src,msg);
			if(0 == bypass)
			{
				if(m_route_func){
					dest = m_route_func(src,msg);
				}

			}else{
				dest = src;
			}
			if(ack && (dest != CHAN_NULL)){
				AckCommMsg(msg,dest,ack);
			}
		}
		cmd_head = cmd_head->next;
	}
	if(!has_cmd){ //如果没有命令被注册则检查自动更新变量

		if(m_route_func){
			dest = m_route_func(src,msg);
		}

		if(src == dest){
			if(Var_Auto_Update_Service(msg)){ //更新变量成功才返回数据
				if(ack && (dest != CHAN_NULL)){
					AckCommMsg(msg,dest,ack);
				}
			}
		}else{
			AckCommMsg(msg,dest,ack);
		}	
		

	}


	return 1;
	
}
static void Init_CmdList(void)
{
	u8 i = 0,j=0;
	for(i=0; i < MAX_DIR_SIZE; i++)
	{
		for(j=0;j<MAX_CMD_NUM;j++)
		{
			cmdlist[i][j].next   = NULL;
			var_list[i].reg_flag = 0;
			var_list[i].var_addr = 0;
			var_list[i].var_size = 0;
			var_list[i].var_type = 0;
		}

	}
	if(dev==NULL){
		dev = Param_dev_Request(PARAM_DEV_NAME);
	}
	//memset(notify_list,0,sizeof(TCmdNotifyList)*MAX_DIR_SIZE);
}

static void Dump_Msg(TCommMsg* msg)
{
	DebugPf("cmd=%d\r\n",		msg->cmd);
	DebugPf("dir=%d\r\n",		msg->dir);
	DebugPf("dev_id=%d\r\n",	msg->dev_id);
	DebugPf("param_u8=%d\r\n",	msg->param.u8_val);
	DebugPf("param_u16=%d\r\n",	msg->param.u16_val);
	DebugPf("param_u32=%d\r\n",	msg->param.u32_val);
	DebugPf("param_f32=%f\r\n",	msg->param.f32_val);
	
}

static u8 CheckSum(u8* buf,u32 len)
{
	u32   i   = 0;
	u8 sum = 0;
	
	for(i = 0;i < len;i++)
	  sum += buf[i];
	return sum;

}
static void AckCommMsg(TCommMsg * msg,TDataChan dest,ack_func ack)
{
	u8 len = BuildDebugMsgBuf(msg,m_send_buf,sizeof(TCommMsg));
	ack(dest,m_send_buf,len);
}

void   ParseDebugCommMsg(TDataChan src,u8* data, u32 len,ack_func ack)
{
	u32 left = len;
	u32 i = 0;
	TCommMsg __CommMsg;
	
	if(len < MSG_LEN) return ;

	for(i = 0; (i < len && left >= 11); i++)
	{
		if((data[i] == 0xfe)&&(data[i+1] == 0x7f))
		{
			//if(CheckSum(data+i,10) == data[i+10])
			{
				#if 0
					memcpy(&__CommMsg,&data[i],10);
				#else				
					__CommMsg.head1  = 0xFE;
					__CommMsg.head2  = 0x7F;
					__CommMsg.dev_id = (data[i + 2]<<8) + data[i + 3];
					__CommMsg.dir 	 = data[i + 4];
					__CommMsg.cmd 	 = data[i + 5];
					__CommMsg.param.__pad.pad[0] = data[i + 9];
					__CommMsg.param.__pad.pad[1] = data[i + 8];
					__CommMsg.param.__pad.pad[2] = data[i + 7];
					__CommMsg.param.__pad.pad[3] = data[i + 6];
				#endif

				
				if(Cmd_Service(src,&__CommMsg,ack))
				{		
				
				}
				
						
				
				left -= 11;
				i    += 10;
				continue;
			}
			
		}
		
		left--;
	}
	
	

	
}
/*
build debug msg buf 
*/
u8	  BuildDebugMsgBuf(TCommMsg* msg, u8* buf, u8 len)
{
	if( (msg == NULL) || (buf == NULL) || (len < sizeof(TCommMsg))) return 0;

	buf[0] = 0xFE;
	buf[1] = 0x7F;
	buf[2] = msg->dev_id>>8;
	buf[3] = msg->dev_id&0xff;
	buf[4] = msg->dir;
	buf[5] = msg->cmd;
	buf[6] = msg->param.__pad.pad[3];
	buf[7] = msg->param.__pad.pad[2];
	buf[8] = msg->param.__pad.pad[1];
	buf[9] = msg->param.__pad.pad[0];

	buf[10]= CheckSum(buf,10);

	return 11;
}




