#include "includes.h"
#include "led.h"
#include "gpio.h"
#if (DEVICE_STM32_LED_EN > 0)
#define GPIO_MODULE
typedef struct{
	int			  gpio;
	bool		  active_low;
	struct LED_Device	  led;
}STM32LED;

#define LSW100_LED(_gpio) \
		{ .gpio = _gpio, .active_low = 1}

static STM32LED stm32_led_list[] = {
	LSW100_LED(GPIO_NR(PE,2)),
	LSW100_LED(GPIO_NR(PE,3)),
	LSW100_LED(GPIO_NR(PE,4)),
	LSW100_LED(GPIO_NR(PE,5)),
};


#define LED1 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|2
#define LED2 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|3
#define LED3 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|4
#define LED4 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|5

unsigned int mode[] = {
		LED1,
		LED2,
		LED3,
		LED4,
};

STM32LED* stm32_led ;

static void stm32_led_set(struct LED_Device* led,int value)
{
	stm32_led = (STM32LED*)led->private_data;

	if(value){
		gpio_set_value(stm32_led->gpio,1);
	}else{
		gpio_set_value(stm32_led->gpio,0);
	}
		
}
static void stm32_led_hardinit(void)
{

	for(int i = 0; i < ARRAY_SIZE(mode); i++){
		gpio_set_mode(mode[i]);
	}
	//默认情况下led 是熄灭的
	for(int i = 0; i < ARRAY_SIZE(stm32_led_list); i++)
	{
		gpio_set_value(stm32_led_list[i].gpio,1);

	}	
}
static int stm32_led_init(void)
{
	
	stm32_led_hardinit();
	for(int i = 0; i < ARRAY_SIZE(stm32_led_list); i++)
	{
		stm32_led_list[i].led.private_data = &stm32_led_list[i];
		stm32_led_list[i].led.set = stm32_led_set;
		led_register(&(stm32_led_list[i].led));
	}
	return 1;
}
device_initcall(stm32_led_init);
#endif
