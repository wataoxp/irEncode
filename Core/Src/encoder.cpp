/*
 * encoder.cpp
 *
 *  Created on: Feb 17, 2026
 *      Author: wataoxp
 */

#include "encoder.h"
#include "gpio.h"
#include <stdlib.h>

Encoder::Encoder(TIM& tim,TIM_TypeDef *Tim) :timer(tim),TIMx(Tim)
{
	;
}

uint32_t Encoder::PinInit(GPIO_TypeDef* A_Port,uint32_t A_Pin,uint32_t A_AF,GPIO_TypeDef* B_Port,uint32_t B_Pin,uint32_t B_Af)
{
	GPIO TI1(A_Port,A_Pin);
	GPIO TI2(B_Port,B_Pin);

	uint32_t ret = 0;

	ret += TI1.Begin();
	TI1.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	TI1.AlternateInit(A_AF);

	ret += TI2.Begin();
	TI2.SetParameter(LL_GPIO_PULL_NO, LL_GPIO_MODE_ALTERNATE, LL_GPIO_SPEED_FREQ_LOW, LL_GPIO_OUTPUT_PUSHPULL);
	TI2.AlternateInit(B_Af);

	return ret;
}

void Encoder::Init(uint32_t Division,uint32_t Filter,uint32_t Mode)
{
	TimerPeripheral::TIM_InputStruct ti1,ti2;

	// 16M/32=0.5MHz. 0,5/8=62kHz
	ti1.Channel = LL_TIM_CHANNEL_CH1;
	ti1.CCxS = LL_TIM_ACTIVEINPUT_DIRECTTI;
	ti1.Prescale = LL_TIM_ICPSC_DIV1;
	ti1.Filter = Filter;
	ti1.Polarity = LL_TIM_IC_POLARITY_RISING;

	ti2.Channel = LL_TIM_CHANNEL_CH2;
	ti2.CCxS = LL_TIM_ACTIVEINPUT_DIRECTTI;
	ti2.Prescale = LL_TIM_ICPSC_DIV1;
	ti2.Filter = Filter;
	ti2.Polarity = LL_TIM_IC_POLARITY_RISING;

	// fDTSをさらに分周。62/4で約15kHzのフィルタ
	LL_TIM_SetClockDivision(TIMx, Division);

	timer.ConfigEncoderMode(&ti1, &ti2, Mode);
}

int16_t Encoder::GetCount(void)
{
	uint16_t Count = 0;

	while(LL_TIM_IsActiveFlag_CC1(TIMx) == 0 || LL_TIM_IsActiveFlag_CC2(TIMx) == 0);
	LL_TIM_ClearFlag_CC1(TIMx);
	LL_TIM_ClearFlag_CC2(TIMx);

	Count = LL_TIM_GetCounter(TIMx);

	// 符号付き16ビットの最大値を超えているなら、これを負の数と見る
	return (Count > INT16_MAX)? (int16_t)Count:Count;
}

// CNTをチェックし、値の変動が4を超えるか否かを返す
bool Encoder::CheckValueMode3(void)
{
	int16_t count = (int16_t)LL_TIM_GetCounter(TIMx);

	if(abs(count) >= EncoderUnits::AbsoluteValueisMode3)
	{
		return true;
	}
	else
	{
		return false;
	}
}


