/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "includes.h"
#include "clock.h"
#include "stm32f10x.h"

static uint64_t imx_clocksource_read(void)
{
	return TIM_GetCounter(TIM6);
}

static struct clocksource cs = {
	.read	= imx_clocksource_read,
	.mask	= CLOCKSOURCE_MASK(16),
	.shift	= 10,
};
static int clocksource_init (void)
{
	u32 tim6_clock = 0;
	
	TIM_TimeBaseInitTypeDef  T; //创建定时器变量
	//以下对该模块的所有操作都必须基于开启了该模块的时钟，否则写寄存器都不能成功
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	//给定时器4分配时钟APB1	//APB1预分频为非1时,定时器时钟频率为2倍频即36*2=72MHZ
	
	TIM_DeInit(TIM6);	//这里只是复位模块状态，没有禁用时钟
	T.TIM_Period 	= 0xFFFF;//定时器为16位的最大值65535
	T.TIM_Prescaler = 0; //不分频 		
	T.TIM_ClockDivision = 0;					//时钟分割 在定时器预分频之前作一段延时，很少用到，设为0即可
	T.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数		UP：0 到TIM_Period的计数	DOWN:0xFFFF ---TIM_Period;
	TIM_TimeBaseInit(TIM6, &T); 				//根据上面设置初始化定时器TIM2
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);		//清空溢出中断
		
   	TIM_SetCounter(TIM6,0);							//设置计数器初值为0	//目前尚未启用该定时器
	TIM_Cmd(TIM6,ENABLE);

//tim_x的时钟= APB1的时钟x 2
//如果APB1 不分频，则tim_x的时钟等于PCLK1的时钟,否则等于pclk * 2
	if(get_APB1_prevdiv() > 0){
		tim6_clock = Get_SyS_CLK(3)<<1;
	}else{
		tim6_clock = Get_SyS_CLK(3);
	}
	cs.mult = clocksource_hz2mult( tim6_clock, cs.shift);

	init_clock(&cs);

	return 1;
}

core_initcall(clocksource_init);



