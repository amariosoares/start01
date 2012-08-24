#include "includes.h"
#include "ethernetif.h"
#include "netconf.h"
#include "pbuf.h"
#include "lwip/ip_addr.h"
#include "tcp.h"
#include "udp.h"
#include "debugcomm.h"
#include "network.h"
#include "gpio.h"
#include "clock.h"

#define SYSTEMTICK_PERIOD_MS  10

__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
static u8 udp_send_ad_flag = 0;
static u8 net_init_ok = 0;
static int int_var1,int_var2,int_var3,int_var4;
static float float_var1,float_var2,float_var3,float_var4;

void Time_Update(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
}

#if ( (DEVICE_NETWORK_EN > 0) && (SYS_MODE == SYS_MODE_MASTER))
#define SERVER_PORT  8000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t timingdelay;
static err_t socket_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
struct send_state
{
  char *data;
  u32_t left;
};
struct tcp_pcb *server_pcb = NULL;
struct tcp_pcb *peer_pcb = NULL;
struct udp_pcb* udp_srv_pcb = NULL;
struct ip_addr   dst_ip;
u16_t  dst_port;

static int send_len = 0;

typedef struct{
	struct kfifo* send_fifo; //将未发送成功的数据丢进去
	struct tcp_pcb *pcb;
}TCP_Sender;

void network_set_ok(u8 ok)
{
	net_init_ok = ok;
}

/*
可以在这里做一个缓存，将tcp_write 未成功的数据
放在里面，等tcp_sent或者tcp_poll中再发送出去.
使用tcp_arg(pcb,TCP_Sender)放进去。
*/
static int send_data(struct tcp_pcb *pcb, u8* data, u32 data_len)
{
  err_t err;
  u16_t len;

  /* We cannot send more data than space available in the send
     buffer. */

  send_len = data_len;
  if (tcp_sndbuf(pcb) < data_len)
  {
    len = tcp_sndbuf(pcb);
  }
  else
  {
    len = data_len;
  }

  err = tcp_write(pcb, data, len, 0);
  if (err == ERR_OK)
  {
  	  
	  err = tcp_output(pcb);
	  tcp_sent(pcb,socket_sent);
	  if (err == ERR_OK) 
  	  	return len;
  }else if (err == ERR_MEM) {
    /* If ERR_MEM, we wait for sent_tcp or poll_tcp to be called
       we do NOT return to the application thread, since ERR_MEM is
       only a temporary error! */

    /* tcp_enqueue returned ERR_MEM, try tcp_output anyway */
    err = tcp_output(pcb);	
	tcp_sent(pcb,socket_sent);
  }
  return 0;
}

static err_t socket_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
  send_len -= len;
  if(send_len > 0){
  		printf("left len=%d need send\n",send_len);
  }
  return ERR_OK;
}

u32 network_send_data(TDataChan chan,u8* data, u32 len)
{
	if(!net_init_ok) return 0; 
	if(peer_pcb &&  (peer_pcb->state== ESTABLISHED)){
		if(chan == CHAN_ETH0)
			return send_data(peer_pcb,data,len);
	}
	return 0;
}


//static char* send_text = "hello world\r\n";
#define BUF_SIZE 128
static u8 recv_buf[BUF_SIZE];

static err_t socket_recv(void *arg, struct tcp_pcb *pcb,	struct pbuf *p, err_t err)
{
  
  struct pbuf *q;
  u32 len = 0;
  u32 copy_len = 0;
  if (err == ERR_OK && p != NULL)
  {
	/* Inform TCP that we have taken the data */
	tcp_recved(pcb, p->tot_len);
  }
//fix me 
  for(q=p; q != NULL; q = q->next) 
  {
    	copy_len = (q->len > (BUF_SIZE-len) )? BUF_SIZE-len:q->len;
  	memcpy(recv_buf + len, q->payload, copy_len);
	len += copy_len;	
  }
  ParseDebugCommMsg(CHAN_ETH0,recv_buf,len, network_send_data);
  if(p) pbuf_free(p);

  return ERR_OK;
 
}
static void socket_err(void *arg, err_t err)
{

	
}

/**
  * @brief  closes tcp connection
  * @param  pcb: pointer to a tcp_pcb struct
  * @param  hs: pointer to a http_state struct
  * @retval
  */
static void close_conn(struct tcp_pcb *pcb)
{

  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, NULL);
  tcp_close(pcb);
}
/*
在某个pcb上没有数据发送也没有数据接收的时候，
也就是该pcb空闲的时候，会间隔性的调用此函数
其用途有几点:
	1.当tcp_write发送数据的时候，可能遇到内存不够
	那么就会返回内存错误，这个时候，我们把需要
	发送的数据缓存起来，在这个事件里面再发出去。
	2.当tcp_close的时候因为还有数据在发送，或者内存
	不够也有可能关闭不成功，可以在这里再来关闭
	
*/
static err_t socket_poll(void *arg, struct tcp_pcb *pcb)
{
  if (arg == NULL)
  {
    //tcp_close(pcb);
  }
  else
  {
  
  }
  return ERR_OK;
}

static err_t socket_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{


  /* Tell TCP that we wish to be informed of incoming data by a call
     to the http_recv() function. */
  peer_pcb = pcb;
  tcp_recv(pcb, socket_recv);
  tcp_err(pcb, socket_err);
  tcp_poll(pcb, socket_poll, 10);

  return ERR_OK;
}
u32 udp_send_data(u8* data, u32 len)
{
	if(udp_srv_pcb && net_init_ok){
		struct pbuf* p = NULL;
	
		    p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
		    if (p == NULL) {
		   	      return 0;
		    } else {
			      p->payload = (void*)data;
			      p->len = p->tot_len = len;
		    }

		 //udp_sendto(udp_srv_pcb,p,&dst_ip,5000); //如果没有用udp_connect 
		 // 则必需指定端口和ip
		 udp_send(udp_srv_pcb,p);

		pbuf_free(p);
	}
		return 0;
}
//当udp收到数据后会自动调用此函数
void client_udp_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p,struct ip_addr *addr, u16_t port)
{
	
	printf("udp [0x%x] recv from %d.%d.%d.%d port:%d\n",upcb,ip4_addr1(addr), \
										ip4_addr2(addr), \
										ip4_addr3(addr), \
										ip4_addr4(addr),  port);
	
}
#define  BROAD_ADDR 255
static int udp_test_init(void)
{
	udp_srv_pcb = udp_new();
	 if (!udp_srv_pcb)
	  {
		return 0;
	  }	
	 #ifdef BROAD_SEND
	 IP4_ADDR(&dst_ip, 192, 168, 50, BROAD_ADDR);
	 #else
	 IP4_ADDR(&dst_ip, 192, 168, 50, 15);
	 #endif
	 
	 dst_port = 5000;
	 
	udp_connect(udp_srv_pcb,&dst_ip,dst_port);
	 udp_recv(udp_srv_pcb,client_udp_recv,NULL);
	 
	 return 1;
}

#if 1
static int network_server_init()
{
  /*create new pcb*/
  
  server_pcb = tcp_new();
  if (!server_pcb  ||   !(udp_test_init()))
  {
	return 0;
  }
  /* bind HTTP traffic to pcb */
  tcp_bind(server_pcb, IP_ADDR_ANY, SERVER_PORT);
  /* start listening on port 80 */
  server_pcb = tcp_listen(server_pcb);
  /* define callback function for TCP connection setup */
  tcp_accept(server_pcb, socket_accept);
	
  net_init_ok = 1;
  return 1;
}
#else


#endif

#ifdef net_dbg
char buf[700]={0xab,0xcd,};
static void net_speed_test()
{
	static __IO uint32_t PreTime = 0; /* this variable is used to create a time reference incremented by 10ms */
	if(LocalTime-PreTime >= 10){
		PreTime = LocalTime;
	  	network_send_data(CHAN_ETH0,buf,700);
	}

}
#endif
void System_Periodic_Handle(void)
{

  /* LwIP periodic services are done here */
  LwIP_Periodic_Handle(LocalTime);

  #ifdef net_dbg
  net_speed_test();
  #endif

}

void network_srv(void)
{
	if(!net_init_ok) return;
	if (ETH_GetRxPktSize()!=0)
	{ 
	  /* process received eth packet */
	  LwIP_Pkt_Handle();
	}
	  /* Periodic tasks */
	System_Periodic_Handle();
}
static u8 net_cmd_parse(TDataChan *src,TCommMsg* msg)
{
	switch(msg->dir)
	{
		case DIR_READ:
			if(msg->cmd == CMD_UDP_IP)
			{
				
			}else if(msg->cmd == CMD_UDP_PORT){
				
			}
			break;
		case DIR_WRITE:
			if(msg->cmd == CMD_UDP_IP)
			{
				
			}else if(msg->cmd == CMD_UDP_PORT){
				
			}
			break;
		default:
			break;
	}
}
static struct TCmd cmd_list[] = {
	CMD_ITEM(DIR_READ, CMD_UDP_IP,	  	  net_cmd_parse),	
	CMD_ITEM(DIR_READ, CMD_UDP_PORT,	  net_cmd_parse),		
	CMD_ITEM(DIR_WRITE, CMD_UDP_IP,	  net_cmd_parse),	
	CMD_ITEM(DIR_WRITE, CMD_UDP_PORT,	  net_cmd_parse),	
};
static  TVarItem var_list[]={
		{CMD_READ_INT1,sizeof(int),&int_var1,VAR_UPDATE_MEMORY},	
		{CMD_READ_INT2,sizeof(int),&int_var2,VAR_UPDATE_MEMORY},
		{CMD_READ_INT3,sizeof(int),&int_var3,VAR_UPDATE_MEMORY},
		{CMD_READ_INT4,sizeof(int),&int_var4,VAR_UPDATE_MEMORY},
		{CMD_READ_FLOAT1,sizeof(float),&float_var1,VAR_UPDATE_MEMORY},
		{CMD_READ_FLOAT2,sizeof(float),&float_var2,VAR_UPDATE_MEMORY},
		{CMD_READ_FLOAT3,sizeof(float),&float_var3,VAR_UPDATE_MEMORY},
		{CMD_READ_FLOAT4,sizeof(float),&float_var4,VAR_UPDATE_MEMORY},
};
void network_var_init(void)
{
	
	RegisterAutoVarList(var_list,ARRAY_SIZE(var_list));
}

#define PHY_RESET_PIN_MODE		GPIO_PORTB|GPIO_OUT_PP|GPIO_50MHZ|5
#define PHY_RESET_PIN           GPIO_NR(PB,5)

void phy_reset(void)
{
	gpio_set_mode(PHY_RESET_PIN_MODE);
	gpio_set_value(PHY_RESET_PIN,1);
	gpio_set_value(PHY_RESET_PIN,0);
	mdelay(100); 
	gpio_set_value(PHY_RESET_PIN,1);
	mdelay(100); 
	gpio_set_value(PHY_RESET_PIN,0);
	mdelay(100); 
	gpio_set_value(PHY_RESET_PIN,1);
	
}

void init_param(void)
{

}
int network_init(void)
{
	DEBUG_FUNC();

	network_var_init();
	//phy_reset();
	/* Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources */
	if(!System_Setup()){
		net_init_ok = 0;
		return 0;
	}
  
	/* Initilaize the LwIP stack */
	LwIP_Init();
	/* Infinite loop */

	//return mongoo_start();

	//IAP_httpd_init();

	net_init_ok = 1;

	//RegisterAutoVar(CMD_NET_SEND_AD,1,&udp_send_ad_flag,VAR_UPDATE_MEMORY);
	
	return network_server_init();
}
device_initcall(network_init);

#endif
