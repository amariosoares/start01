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
static struct TSerialDevice* uart0;
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
static int test_fm25l16(void)
{
	static DWORD v = 0;
	static int addr = 0;
	Param_WriteInteger(addr%2048,v);
	printf("addr %d write 0x%x\n",addr%2048,v);
	
	
	v = 0;
	v = Param_ReadInteger(addr%2048,0);

	
	printf("addr %d read 0x%x\n",addr%2048,v);
	v++;
	addr++;
}
static int test_uart(void)
{
	
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

	uart0 = usart_request("ttystm1");
	if(uart0){
		if(usart_open(uart0,&uartx)){

		}
	}
	register_console(uart0);
	
	return 1;
}
module_initcall(test_suite_init);
#endif