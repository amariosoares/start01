#include "includes.h"
#include "pinmux.h"
#include "gpio.h"


#define gpio_get_pin(gpio) (1 << ((gpio%16)&0xf))

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

