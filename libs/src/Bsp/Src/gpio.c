#include "includes.h"
#include "gpio.h"


#define gpio_get_pin(gpio) (1 << ((gpio%16)&0xf))
void stm32_gpio_mode(int gpio_mode);
GPIO_TypeDef* stm32_gpio_getbase(int gpio);
void gpio_set_value(unsigned gpio, int value)
{
	GPIO_TypeDef* base = stm32_gpio_getbase(gpio);
	if(base == NULL) return;

	if(value)
		GPIO_SetBits(base,gpio_get_pin(gpio));
	else
		GPIO_ResetBits(base,gpio_get_pin(gpio));
}
int gpio_direction_input(unsigned gpio)
{
	
	return 0;
}
int gpio_direction_output(unsigned gpio, int value)
{
	return 0;
}
int gpio_get_value(unsigned gpio)
{
	GPIO_TypeDef* base = stm32_gpio_getbase(gpio);
	
	if(base == NULL) return -1;
		
	return GPIO_ReadInputDataBit(base,gpio_get_pin(gpio));
}
#define GET_GPIO_VALUE(port) (4<<port)
static GPIO_TypeDef* GPIO_BASE[] = {
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG
};
#define gpio_count ARRAY_SIZE(GPIO_BASE)*16
static GPIOSpeed_TypeDef GPIO_SPEED[] = {
	GPIO_Speed_10MHz,
	GPIO_Speed_2MHz,
	GPIO_Speed_50MHz
};

static GPIOMode_TypeDef GPIO_MODE[] = {
	GPIO_Mode_Out_PP,
  	GPIO_Mode_AIN,
  	GPIO_Mode_IN_FLOATING,
  	GPIO_Mode_IPD,
  	GPIO_Mode_IPU,
  	GPIO_Mode_Out_OD,
  	GPIO_Mode_AF_OD,
  	GPIO_Mode_AF_PP,
};
void gpio_set_mode(int gpio_mode)
{
	unsigned int pin  =  gpio_mode & GPIO_PIN_MASK;
	unsigned int port = (gpio_mode & GPIO_PORT_MASK) >> GPIO_PORT_SHIFT;
	unsigned int mode = (gpio_mode & GPIO_MODE_MASK) >> GPIO_MODE_SHIFT;
	unsigned int speed= (gpio_mode & GPIO_SPEED_MASK) >> GPIO_SPEED_SHIFT;

	
	GPIO_InitTypeDef GPIO_InitStructure;		//串口引脚结构


	RCC_APB2PeriphClockCmd(GET_GPIO_VALUE(port), ENABLE);

	GPIO_InitStructure.GPIO_Pin   = (1 << pin);	//串口发送引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_MODE[mode];	//推挽输出千万记得不要设置为复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_SPEED[speed];	//频率50MHz
	
	GPIO_Init(GPIO_BASE[port], &GPIO_InitStructure);//初始化引脚
	
}
GPIO_TypeDef* stm32_gpio_getbase(int gpio)
{
	if(gpio > gpio_count) return NULL;

	return GPIO_BASE[gpio/16];

}


