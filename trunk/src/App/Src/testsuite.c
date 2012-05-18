#include "includes.h"
#include "led.h"
#include "clock.h"
#include "timer.h"
#include <ctype.h>
#if (TEST_MODULE_EN > 0)
const char send_text[] = {0xab,0xcd,0,1,2,3,4};
static int test_led(void)
{
#if 1	

		led_revert(1);
		led_revert(2);
		led_revert(3);
		led_revert(4);
		led_revert(5);
		led_revert(6);
		led_revert(7);
		led_revert(8);
		network_send_data(CHAN_ETH0,(u8 *)(send_text),sizeof(send_text));
		//mdelay(1000);

#endif
	return 1;
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

	return 1;
}
module_initcall(test_suite_init);
#endif
