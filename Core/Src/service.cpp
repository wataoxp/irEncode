/*
 * service.cpp
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */

#include "service.h"
#include "config.h"
#include "isr.h"
#include <stdio.h>
#include <string.h>

void UpdateSeg(RealClock& rtc,TM1652& seg,RealClockSpace::TimeRegBitField *tr)
{
	if((tr->SecondTens + tr->SecondUnits) != 0)
	{
		// 0秒でないなら再読み込み
		__NOP();
		LL_RTC_ClearFlag_RS(RTC);
		while(LL_RTC_IsActiveFlag_RS(RTC) != 1);
		rtc.GetTimeRegister(tr);
	}

	seg.WriteDig(tr->HourTens, tr->HourUnits, tr->MinuteTens, tr->MinuteUnits);
}

void UpdateDisplay(SO1602& lcd,RealClockSpace::TimeRegBitField *tr)
{
	uint8_t ht,hu,mnt,mnu,st,su;
	char str[30];

	ht = tr->HourTens;
	hu = tr->HourUnits;
	mnt = tr->MinuteTens;
	mnu = tr->MinuteUnits;
	st = tr->SecondTens;
	su = tr->SecondUnits;

	sprintf(str,"%d%d:%d%d.%d%d",ht,hu,mnt,mnu,st,su);
	lcd.PointClear(0);
	lcd.StringLCD(str, strlen(str));
}

void SetUserTime(TM1652& seg,Encoder& encode,RealClock& rtc,RealClockSpace::Options wut,uint32_t timing)
{
	LL_TIM_SetCounter(WatchClock::EncoderTimer, 0);
	LL_TIM_ClearFlag_CC1(WatchClock::EncoderTimer);

	LL_SYSTICK_EnableIT();

	uint32_t hour = SetTimeLoop(seg, encode, 0, 23, 0, true);
	uint32_t minute = SetTimeLoop(seg, encode, 0, 59, hour, false);

	LL_SYSTICK_DisableIT();

	rtc.SetTime(LL_RTC_TIME_FORMAT_AM_OR_24, hour, minute, 0);
	seg.WriteDig(hour/10, hour%10, minute/10, minute%10,false);


	if(wut == RealClockSpace::Options::WakeUp_Enable)
	{
		rtc.SetWakeUpTimer(timing);
	}

	__NVIC_DisableIRQ(EXTI4_15_IRQn);
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_14);
	LL_EXTI_DisableFallingTrig_0_31(LL_EXTI_LINE_14);
}

uint32_t SetTimeLoop(TM1652& seg,Encoder& encode,uint32_t min,uint32_t max,uint32_t FixedHour,bool isHour)
{
	int16_t count = 0;
	uint32_t value = 0;
	const ISR_Flags *flag = GetFlagStruct();

//	while(!GetExtiFlag())
	while(!flag->ExtiFlag)
	{
		count = 0;

		while(!encode.CheckValueMode3() && !flag->ExtiFlag);
		count = (int16_t)LL_TIM_GetCounter(WatchClock::EncoderTimer);
		LL_TIM_SetCounter(WatchClock::EncoderTimer, 0);

		if(flag->ExtiFlag) break;

		if(count > 0)
		{
			value = (value < max)? value+1:min;
		}
		else
		{
			value = (value > min)? value-1:max;
		}

		if(isHour)
		{
			seg.WriteDig(value/10, value%10, 0,0, false);
		}
		else
		{
			seg.WriteDig(FixedHour/10,FixedHour%10,value/10,value%10,true);
		}

		DelayMode::Standard::mDelay(100);

	}
	if(isHour)	seg.WriteDig(value/10, value%10, 0, 0,true);		// 時間の設定完了をドットで示す
	SetExtiFlag(false);

	return value;
}


