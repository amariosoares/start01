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
	
	TIM_TimeBaseInitTypeDef  T; //������ʱ������
	//���¶Ը�ģ������в�����������ڿ����˸�ģ���ʱ�ӣ�����д�Ĵ��������ܳɹ�
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	//����ʱ��4����ʱ��APB1	//APB1Ԥ��ƵΪ��1ʱ,��ʱ��ʱ��Ƶ��Ϊ2��Ƶ��36*2=72MHZ
	
	TIM_DeInit(TIM6);	//����ֻ�Ǹ�λģ��״̬��û�н���ʱ��
	T.TIM_Period 	= 0xFFFF;//��ʱ��Ϊ16λ�����ֵ65535
	T.TIM_Prescaler = 0; //����Ƶ 		
	T.TIM_ClockDivision = 0;					//ʱ�ӷָ� �ڶ�ʱ��Ԥ��Ƶ֮ǰ��һ����ʱ�������õ�����Ϊ0����
	T.TIM_CounterMode = TIM_CounterMode_Up; 	//���ϼ���		UP��0 ��TIM_Period�ļ���	DOWN:0xFFFF ---TIM_Period;
	TIM_TimeBaseInit(TIM6, &T); 				//�����������ó�ʼ����ʱ��TIM2
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);		//�������ж�
		
   	TIM_SetCounter(TIM6,0);							//���ü�������ֵΪ0	//Ŀǰ��δ���øö�ʱ��
	TIM_Cmd(TIM6,ENABLE);

//tim_x��ʱ��= APB1��ʱ��x 2
//���APB1 ����Ƶ����tim_x��ʱ�ӵ���PCLK1��ʱ��,�������pclk * 2
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



