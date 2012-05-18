#ifndef LED_H
#define LED_H

/*
这里为什么不直接定义led的状态，是因为led有许多种，可能有
三色的led，他的状态就不止亮和灭
*/
typedef enum{
	LED_ON=0,
	LED_OFF
}LEDState;

struct LED_Device{
	u8    num;
	int   max_value;
	int   cur_value;
	void (*set)(struct LED_Device*, int value);
	void* private_data;
};

void led_revert(u8 num);
void led_set(u8 num, int value);
int  led_num(void);
BOOL led_register(struct LED_Device* dev);
#endif
