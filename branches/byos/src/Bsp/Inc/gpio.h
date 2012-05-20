#ifndef GPIO_H
#define GPIO_H
#include "pinmux.h"

void gpio_set_value(unsigned gpio, int value);
int  gpio_get_value(unsigned gpio);
int  gpio_direction_input(unsigned gpio);
int  gpio_direction_output(unsigned gpio, int value);

#endif
