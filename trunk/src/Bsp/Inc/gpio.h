#ifndef GPIO_H
#define GPIO_H
//#include "pinmux.h"

#define GPIO_NR(bank, nr)		((bank) * 16 + (nr))

//max pin = 16
#define GPIO_PIN_MASK 	0x0f


//max port = 8
#define GPIO_PORT_SHIFT 4
#define GPIO_PORT_MASK (0x7 << GPIO_PORT_SHIFT)

#define GPIO_MODE_SHIFT 7
#define GPIO_MODE_MASK (0x7 << GPIO_MODE_SHIFT)

#define GPIO_SPEED_SHIFT  10
#define GPIO_SPEED_MASK  (0x3 << GPIO_SPEED_SHIFT)

#define PA  0
#define PB  1
#define PC  2
#define PD  3
#define PE  4
#define PF  5
#define PG  6

#define GPIO_PORTA 			(0 << GPIO_PORT_SHIFT)
#define GPIO_PORTB 			(1 << GPIO_PORT_SHIFT)
#define GPIO_PORTC 			(2 << GPIO_PORT_SHIFT)
#define GPIO_PORTD 			(3 << GPIO_PORT_SHIFT)
#define GPIO_PORTE 			(4 << GPIO_PORT_SHIFT)
#define GPIO_PORTF 			(5 << GPIO_PORT_SHIFT)
#define GPIO_PORTG			(6 << GPIO_PORT_SHIFT)


#define  GPIO_OUT_PP		(0 << GPIO_MODE_SHIFT)	
#define  GPIO_AIN 			(1 << GPIO_MODE_SHIFT)	
#define  GPIO_IN_FLOATING   (2 << GPIO_MODE_SHIFT)	
#define  GPIO_IPD			(3 << GPIO_MODE_SHIFT)	
#define  GPIO_IPU 			(4 << GPIO_MODE_SHIFT)	
#define  GPIO_OUT_OD		(5 << GPIO_MODE_SHIFT)	
#define  GPIO_AF_OD 		(6 << GPIO_MODE_SHIFT)	
#define  GPIO_AF_PP			(7 << GPIO_MODE_SHIFT)	
  
#define  GPIO_10MHZ			(0 << GPIO_SPEED_SHIFT)	
#define  GPIO_2MHZ			(1 << GPIO_SPEED_SHIFT)	
#define  GPIO_50MHZ			(2 << GPIO_SPEED_SHIFT)	

void 	gpio_set_mode(int gpio_mode);
void gpio_set_value(unsigned gpio, int value);
int  gpio_get_value(unsigned gpio);
int  gpio_direction_input(unsigned gpio);
int  gpio_direction_output(unsigned gpio, int value);

#endif
