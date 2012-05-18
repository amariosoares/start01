#include "includes.h"
#include "led.h"
#include "gpio.h"
#if (DEVICE_STM32_LED_EN > 0)
#define GPIO_MODULE
typedef struct{
#ifdef GPIO_MODULE
	int			  gpio;
#else
	GPIO_TypeDef* gpio_base;
	u16			  gpio_pin;
#endif
	bool		  active_low;
	struct LED_Device	  led;
}STM32LED;

#ifndef GPIO_MODULE
#define LSW100_LED(_gpio_base,_gpio_pin) \
	{ .gpio_base = _gpio_base, .gpio_pin= _gpio_pin ,.active_low = 1}
static STM32LED stm32_led_list[] = {
	LSW100_LED(GPIOE, GPIO_Pin_2),
	LSW100_LED(GPIOE, GPIO_Pin_3),
	LSW100_LED(GPIOE, GPIO_Pin_4),
	LSW100_LED(GPIOE, GPIO_Pin_5),
	LSW100_LED(GPIOE, GPIO_Pin_6),
	LSW100_LED(GPIOC, GPIO_Pin_0),
	LSW100_LED(GPIOC, GPIO_Pin_2),
	LSW100_LED(GPIOC, GPIO_Pin_3),
};
#else
#define LSW100_LED(_gpio) \
		{ .gpio = _gpio, .active_low = 1}

static STM32LED stm32_led_list[] = {
	LSW100_LED(STM32_GPIO_NR(PE,2)),
	LSW100_LED(STM32_GPIO_NR(PE,3)),
	LSW100_LED(STM32_GPIO_NR(PE,4)),
	LSW100_LED(STM32_GPIO_NR(PE,5)),
	LSW100_LED(STM32_GPIO_NR(PE,6)),
	LSW100_LED(STM32_GPIO_NR(PC,0)),
	LSW100_LED(STM32_GPIO_NR(PC,2)),
	LSW100_LED(STM32_GPIO_NR(PC,3)),
};


#define LED1 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|2
#define LED2 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|3
#define LED3 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|4
#define LED4 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|5
#define LED5 		GPIO_PORTE|GPIO_OUT_PP|GPIO_50MHZ|6
#define LED6 		GPIO_PORTC|GPIO_OUT_PP|GPIO_50MHZ|0
#define LED7 		GPIO_PORTC|GPIO_OUT_PP|GPIO_50MHZ|2
#define LED8 		GPIO_PORTC|GPIO_OUT_PP|GPIO_50MHZ|3

unsigned int mode[] = {
		LED1,
		LED2,
		LED3,
		LED4,
		LED5,
		LED6,
		LED7,
		LED8
};
#endif

STM32LED* stm32_led ;

static void stm32_led_set(struct LED_Device* led,int value)
{
	stm32_led = (STM32LED*)led->private_data;

	#ifdef GPIO_MODULE
	if(value){
			gpio_set_value(stm32_led->gpio,1);
		}else{
			gpio_set_value(stm32_led->gpio,0);
		}

	#else
	if(value){
			GPIO_SetBits(stm32_led->gpio_base, stm32_led->gpio_pin);		//输出
		}else{
			GPIO_ResetBits(stm32_led->gpio_base, stm32_led->gpio_pin);		//输出
		}

	#endif
	
		
}
static void stm32_led_hardinit(void)
{

#ifndef GPIO_MODULE
	GPIO_InitTypeDef GPIO_InitStructure;		//串口引脚结构

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	for(int i = 0; i < ARRAY_SIZE(stm32_led_list); i++)
	{
		
		GPIO_InitStructure.GPIO_Pin   = stm32_led_list[i].gpio_pin;	//串口发送引脚
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//推挽输出千万记得不要设置为复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//频率50MHz
		
		GPIO_Init(stm32_led_list[i].gpio_base, &GPIO_InitStructure);//初始化引脚
		
		GPIO_SetBits(stm32_led_list[i].gpio_base, stm32_led_list[i].gpio_pin);	
	}
#else
	for(int i = 0; i < ARRAY_SIZE(mode); i++){
		stm32_gpio_mode(mode[i]);
	}
	//默认情况下led 是熄灭的
	for(int i = 0; i < ARRAY_SIZE(stm32_led_list); i++)
	{
		gpio_set_value(stm32_led_list[i].gpio,1);

	}
	

#endif
	
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
