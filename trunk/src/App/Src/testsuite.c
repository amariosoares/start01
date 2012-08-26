#include "includes.h"
#include "led.h"
#include "clock.h"
#include "timer.h"
#include "debugcomm.h"
#include "datetime.h"
#include "paramcfg.h"
#include "serial.h"
#include "gpio.h"

#include <ctype.h>
#if (TEST_MODULE_EN > 0)
static struct Param_Device*  fm25l16_dev = NULL;
static struct Param_Device*  bpk_dev 		= NULL;
static u32 fm25l16_dev_size = 1;
static u32 bpk_dev_size = 1;
static struct TSerialDevice* uart3;
static struct TSerialDevice* uart2;
static struct TSerialDevice* uart1;
static TSerialDesc uartx={
	.baudrate=B115200,
	.mode=SERIAL_RX_INT_MODE |SERIAL_TX_INT_MODE,
};
//anybus
static TSerialDesc uart2_desc={
	.baudrate=B9600,
	.mode=SERIAL_RX_INT_MODE |SERIAL_TX_INT_MODE,
};
//cmbus
static TSerialDesc uart3_desc={
	.baudrate=B9600,
	.mode=SERIAL_RX_INT_MODE , //发送采用查询方式
};
extern struct TSerialDevice* anybus_usart ;
const char send_text[] = {0xab,0xcd,0,1,2,3,4};

#define CMBUS_RXTX_MODE 		GPIO_PORTC|GPIO_OUT_PP|GPIO_50MHZ|12
#define CMBUS_RXTX_GPIO			GPIO_NR(PC,12)
#define CMBUS_RX 				1
#define CMBUS_TX				0


#define ANYBUS_RESET_MODE 		GPIO_PORTD|GPIO_OUT_PP|GPIO_50MHZ|14
#define ANYBUS_RESET_GPIO			GPIO_NR(PD,14)

//#define CMBUS_FO_MODE 		GPIO_PORTD|GPIO_OUT_PP|GPIO_50MHZ|2

static int     int_var1,int_var2,int_var3,int_var4;
static float float_var1,float_var2,float_var3,float_var4;


static int test_led(void)
{
#if 1	
		led_revert(1);
		led_revert(2);
		led_revert(3);
		led_revert(4);
#endif
	return 1;
}
static int test_rtc(void)
{
	TDateTime dt;

	
	if(get_datetime(&dt))
	{
		printf("%d-%d-%d %d:%d:%d\n",dt.year,dt.mon,dt.day,dt.hour,dt.min,dt.sec);
	}
	return 1;
}
static void test_fm25l16(void)
{
	static DWORD v = 0;
	static int addr = 0;
	Param_WriteInteger(fm25l16_dev,addr%fm25l16_dev_size,v);
	printf("addr %d write 0x%x\n",addr%fm25l16_dev_size,v);
	
	
	v = 0;
	v = Param_ReadInteger(fm25l16_dev,addr%fm25l16_dev_size,0);

	
	printf("addr %d read 0x%x\n",addr%fm25l16_dev_size,v);
	v++;
	addr++;
}
static void test_bpk(void)
{
	static DWORD v = 0;
	static int addr = 0;
	Param_WriteInteger(bpk_dev,addr%bpk_dev_size,v);
	printf("addr %d write 0x%x\n",addr%bpk_dev_size,v);
	
	
	v = 0;
	v = Param_ReadInteger(bpk_dev,addr%bpk_dev_size,0);

	
	printf("addr %d read 0x%x\n",addr%bpk_dev_size,v);
	v++;
	addr++;
}
static  const char* usart_text = "hello yangxd\n";
static u8 usart_buf[128];
//for anybus
static void test_uart2(void)
{
	if(uart2){
		u32 len = usart_getpacket(uart2,usart_buf,128);
		if(len){
			usart_sendstring(uart2,usart_buf,len);
		}
	}
}
//for cmbus
static void test_uart3(void)
{
	if(uart3){		
		u32 len = usart_getpacket(uart3,usart_buf,128);
		if(len){
			//将cmbus 设置为发送状态
			gpio_set_value(CMBUS_RXTX_GPIO,CMBUS_TX);
			usart_sendstring(uart3,usart_buf,len);	
			//发送完毕设置为接收状态
			gpio_set_value(CMBUS_RXTX_GPIO,CMBUS_RX);
		}
	}
}

#if (DEVICE_NETWORK_EN > 0)
const char* net_text = "welcome to stm32f107's network!!!";
void network_test(void)
{
	network_send_data(CHAN_ETH0,net_text, strlen(net_text));
}
#endif
#include "abcc.h"
#include "appd.h"
void TestSuiteJob(void *tid , void * arg) 
{
	
#if (DEVICE_NETWORK_EN > 0)
	//network_test();
#endif
	test_led();
if( ( ABCC_eAbccState == ABP_ANB_STATE_PROCESS_ACTIVE) )
{
	static int spd = 0;
 	APPD_iRefSpeed = spd++;
	APPD_ProcessDataChanged();
}
#if 0
	test_rtc();
	test_fm25l16();
	test_bpk();
	if(0){ //如果要测试串口，请打开
		test_uart2();
		test_uart3();
	}
#endif

}
static void init_usart()
{
	uart2= usart_request("ttystm2");
	if(uart2){
		if(!usart_open(uart2,&uart2_desc)){
			printf("ttystm2 open failed\n");
		}
	}
	uart3= usart_request("ttystm4");
	if(uart3){
		if(!usart_open(uart3,&uart3_desc)){
			printf("ttystm3 open failed\n");
		}
		//打开cmbus串口后，首先将其控制为接收状态
		gpio_set_value(CMBUS_RXTX_GPIO,CMBUS_RX);
	}
}
#include "abcc_td.h"
#include "abcc_com.h"
#include "abcc.h"
static int cnt = 0;

extern u16 anybus_recv_len;
static u8 anybus_recv_buf[512];
void anybus_timeout(void *tid , void * arg) 
{
	ABCC_RunTimerSystem();
	if(((cnt++)%5) == 0){
		int len = usart_getdatasize(anybus_usart);
		if(len >= anybus_recv_len){
			usart_getpacket(anybus_usart,anybus_recv_buf,len);
			APDR_SerialTelegramReceived(anybus_recv_buf);
		}
	}
}
void anybus_service(void)
{
	ABCC_RunDriver();
}
static void anybus_init(void)
{
	struct TimerDevice* dev;

	
	gpio_set_value(ANYBUS_RESET_GPIO,0);
	mdelay(100);
	gpio_set_value(ANYBUS_RESET_GPIO,1);
	
	APPC_SetSwitch1( (UINT8)60 );
	APPC_SetSwitch2( (UINT8)3 );
	ABCC_StartDriver();
	
	APPD_ProcessDataChanged();
	dev = request_timer(1);
	
	if(dev == NULL) return ;
	
	if( dev ){
		if(!create_timer(dev,ABCC_TIMER_RESOLUTION_MS* DELAY_MSECOND, anybus_timeout, NULL, LOOP))
			return;
	}
}
u8 test_cmd_parse(TDataChan *src,TCommMsg* msg)
{
	if( msg->dir == DIR_READ)
	{
		if((msg->cmd == CMD_READ_TIME) || (msg->cmd == CMD_READ_DATE))
		{
			TZipDateTimeDef zdt;
			if(get_zipdatetime(&zdt))
			{
				msg->param.u32_val = zdt.value;
			}
			
		}

	}else if(msg->cmd == DIR_WRITE){

		if((msg->cmd == CMD_READ_TIME) || (msg->cmd == CMD_READ_DATE))
		{
			TZipDateTimeDef zdt;
			zdt.value = msg->param.u32_val ;
			set_zipdatetime(&zdt);	
		}
	}
	
	return 0;
}

static  TVarItem var_list[]={
		{CMD_READ_INT1,sizeof(int),&int_var1,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},	
		{CMD_READ_INT2,sizeof(int),&int_var2,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_INT3,sizeof(int),&int_var3,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_INT4,sizeof(int),&int_var4,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_FLOAT1,sizeof(float),&float_var1,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_FLOAT2,sizeof(float),&float_var2,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_FLOAT3,sizeof(float),&float_var3,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
		{CMD_READ_FLOAT4,sizeof(float),&float_var4,VAR_UPDATE_MEMORY|VAR_UPDATE_FLASH},
};
static struct TCmd cmd_list[] = {
	CMD_ITEM(DIR_READ,    CMD_READ_DATE,	 test_cmd_parse),
	CMD_ITEM(DIR_WRITE, CMD_READ_DATE,	  test_cmd_parse),		
	CMD_ITEM(DIR_READ,    CMD_READ_TIME,	 test_cmd_parse),
	CMD_ITEM(DIR_WRITE, CMD_READ_TIME,	  test_cmd_parse),	
}; 

void test_var_init(void)
{
	RegisterAutoVarList(var_list,ARRAY_SIZE(var_list));
	
	RegisterCmdList(cmd_list,ARRAY_SIZE(cmd_list));
	if(fm25l16_dev)
	{
		int_var1 = Param_ReadInteger(fm25l16_dev,GET_OFFSET(CMD_READ_INT1),0);
		int_var2 = Param_ReadInteger(fm25l16_dev,GET_OFFSET(CMD_READ_INT2),0);
		int_var3 = Param_ReadInteger(fm25l16_dev,GET_OFFSET(CMD_READ_INT3),0);
		int_var4 = Param_ReadInteger(fm25l16_dev,GET_OFFSET(CMD_READ_INT4),0);
		float_var1 = Param_ReadFloat(fm25l16_dev,GET_OFFSET(CMD_READ_FLOAT1),0);
		float_var2 = Param_ReadFloat(fm25l16_dev,GET_OFFSET(CMD_READ_FLOAT2),0);
		float_var3 = Param_ReadFloat(fm25l16_dev,GET_OFFSET(CMD_READ_FLOAT3),0);
		float_var4 = Param_ReadFloat(fm25l16_dev,GET_OFFSET(CMD_READ_FLOAT4),0);	
	}
}

static int test_suite_init(void)
{
	u8 ret = 0;
	struct TimerDevice* dev;

	
	DEBUG_FUNC();
	
	dev = request_timer(1);
	
	if(dev == NULL) return 0;
	
	if( dev ){
		if(!create_timer(dev, DELAY_SECOND, TestSuiteJob, NULL, LOOP))
			return 0;
	}
	
	check_rtc_exist();

	uart1 = usart_request("ttystm1");
	if(uart1){
		if(usart_open(uart1,&uartx)){

		}
	}
	register_console(uart1);
	//init_usart();
	anybus_init();
	fm25l16_dev = Param_dev_Request(DEV_FM25L16);
	fm25l16_dev_size = Param_Devsize(fm25l16_dev);
	assert_param(fm25l16_dev_size);
	bpk_dev = Param_dev_Request(DEV_BPK);
	bpk_dev_size = Param_Devsize(bpk_dev);
	assert_param(bpk_dev_size);

	gpio_set_mode(CMBUS_RXTX_MODE);
	gpio_set_mode(ANYBUS_RESET_MODE);

	test_var_init();
	return 1;
}
module_initcall(test_suite_init);
#endif
