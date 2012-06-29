#include "includes.h"
#include "led.h"

#define MAX_LED_NUM   8
static int _led_num = 0;
static struct LED_Device* led_list[MAX_LED_NUM];

static struct LED_Device* led_find(u8 num)
{
	if((num > _led_num) || (num == 0)) return NULL;

	num--;
	
	return led_list[num];
}
void led_set(u8 num, int value)
{
	struct LED_Device* led = led_find(num);
	if(led){
		led->set(led,value);
		led->cur_value = value;
	}
}
void led_revert(u8 num)
{
	struct LED_Device* led = led_find(num);
	if(led){
		led->cur_value ^= 1;
		led->set(led,led->cur_value);
	}
}
int  led_num(void)
{
	return _led_num;
}
BOOL led_register(struct LED_Device* dev)
{
	if(dev){
		if(_led_num > MAX_LED_NUM){
			return FALSE;
		}
		led_list[_led_num++] = dev;
		return TRUE;
	}
	return FALSE;
}
