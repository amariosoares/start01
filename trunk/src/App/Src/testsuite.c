#include "includes.h"
#include "led.h"
#include "clock.h"
#include "timer.h"
#include "debugcomm.h"
#include "datetime.h"
#include "paramcfg.h"
#include "serial.h"

#include <ctype.h>
#if (TEST_MODULE_EN > 0)
static struct Param_Device*  fm25l16_dev = NULL;
static struct Param_Device*  bpk_dev 		= NULL;
static u32 fm25l16_dev_size = 1;
static u32 bpk_dev_size = 1;
static struct TSerialDevice* uart2;
static struct TSerialDevice* uart1;
static TSerialDesc uartx={
	.baudrate=B115200,
	.mode=SERIAL_RX_INT_MODE |SERIAL_TX_INT_MODE,
};
const char send_text[] = {0xab,0xcd,0,1,2,3,4};
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
static void test_uart2(void)
{
	if(uart2){
		u32 len = usart_getpacket(uart2,usart_buf,128);
		if(len){
			usart_sendstring(uart2,usart_buf,len);
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
void TestSuiteJob(void *tid , void * arg) 
{
	
#if (DEVICE_NETWORK_EN > 0)
	//network_test();
#endif
	test_led();
	test_rtc();
	test_fm25l16();
	test_bpk();
	if(0){ //如果要测试串口，请打开
		test_uart2();
	}

}
static void init_usart()
{
	uart2= usart_request("ttystm2");
	if(uart2){
		if(!usart_open(uart2,&uartx)){
			printf("ttystm2 open failed\n");
		}
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
	fm25l16_dev = Param_dev_Request(DEV_FM25L16);
	fm25l16_dev_size = Param_Devsize(fm25l16_dev);
	assert_param(fm25l16_dev_size);
	bpk_dev = Param_dev_Request(DEV_BPK);
	bpk_dev_size = Param_Devsize(bpk_dev);
	assert_param(bpk_dev_size);
	return 1;
}
module_initcall(test_suite_init);
#endif
