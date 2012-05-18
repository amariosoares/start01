#ifndef LED_H
#define LED_H

/*
����Ϊʲô��ֱ�Ӷ���led��״̬������Ϊled������֣�������
��ɫ��led������״̬�Ͳ�ֹ������
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
