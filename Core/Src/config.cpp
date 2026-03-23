/*
 * config.cpp
 *
 *  Created on: Feb 26, 2026
 *      Author: wataoxp
 */

#include "config.h"


uint32_t WatchSetUp(RealClock& rtc,UART& uart,CoreClock source,TM1652& seg)
{
	using namespace WatchClock;
	uint32_t ret = 0;

	// TM1652通信用
	UART_Config(uart, (source*1000*1000), SegPort, TxPin, TxAf, SegPort, RxPin, RxAf);

	seg.Init(1,5);
	seg.WriteDig(1,2,3,4);

	// STOP1モードを有効化
	LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
	LL_LPM_EnableDeepSleep();

	// RTCの設定および割り込みの有効化
	ret = RTC_Config(rtc);
	__NVIC_SetPriority(RTC_TAMP_IRQn, 0);
	__NVIC_EnableIRQ(RTC_TAMP_IRQn);

	// RTC割りこみはEXTIライン19 立ち上がりエッジ固定
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_19);

	return ret;
}

uint32_t EncoderSetUp(TIM& tim,Encoder& encode)
{
	using namespace WatchClock;
	uint32_t ret = 0;

	TIM_Config(tim, 0, UINT16_MAX);

	// もっとも厳しいフィルタ設定
	encode.Init(LL_TIM_CLOCKDIVISION_DIV4, LL_TIM_IC_FILTER_FDIV32_N8, LL_TIM_ENCODERMODE_X4_TI12);
	ret = encode.PinInit(EncoderPort, ChannelA_Pin, ChannelA_Af, EncoderPort, ChannelB_Pin, ChannelB_Af);
	tim.EnableTimer();

	LL_TIM_ClearFlag_CC1(EncoderTimer);
	LL_TIM_ClearFlag_CC2(EncoderTimer);

	return ret;
}

uint32_t ButtonSetUp(void)
{
	using namespace WatchClock;
	uint32_t ret = 0;

	ret += EXTI_Config(EncoderPushPort, EncoderPushPin, LL_GPIO_PULL_UP, LL_EXTI_MODE_IT, LL_EXTI_TRIGGER_FALLING);

	__NVIC_EnableIRQ(EXTI4_15_IRQn);
	__NVIC_SetPriority(EXTI4_15_IRQn, 0);

	return ret;
}

void ConfigLCD(I2C& i2c,CoreClock source,SO1602& lcd)
{
	using namespace WatchClock;
	I2C_Config(source, i2c, SO1602Port, SCLPin, SCLAf, SO1602Port, SDAPin, SDAAf);
	lcd.Init<DelayMode::Standard>(source);
	lcd.SetCusor(0, 0);
	lcd.StringLCD("Clock", 5);
}

uint32_t ConfigDate(RealClock& rtc)
{
	using namespace RealClockSpace;
	ConfigParameters init;

	init.WeekDay = LL_RTC_WEEKDAY_TUESDAY;
	init.Month = LL_RTC_MONTH_FEBRUARY;
	init.Day = 3;
	init.Year = 26;

	return rtc.SetDate(init.WeekDay, init.Month, init.Day, init.Year);
}

uint32_t ConfigAlarm(RealClock& rtc,RealClockSpace::Options alarm,RealClockSpace::Options sel)
{
	using namespace RealClockSpace;
	ConfigParameters init;
	uint32_t ret = 0;

	init.Alarm = alarm;
	init.SelectAlarm = sel;

	if(init.Alarm == Options::Alarm_Enable)
	{
		if((init.SelectAlarm == Options::ALMA) || (init.SelectAlarm == Options::ALMA_ALMB))
		{
			init.ALMA.Hours = 0;
			init.ALMA.Minutes = 0;
			init.ALMA.Seconds = 10;

			init.ALMA.Day = 0;
			init.ALMA.WeekDaySel = AlarmValue::WeekDay_Disable;

			// 日付、時間、分の指定をマスク(無視)
			init.ALMA.Mask = LL_RTC_ALMA_MASK_DATEWEEKDAY | LL_RTC_ALMA_MASK_HOURS
					|LL_RTC_ALMA_MASK_MINUTES;
		}
		if((init.SelectAlarm == Options::ALMB) || (init.SelectAlarm == Options::ALMA_ALMB))
		{
			init.ALMB.Hours = 0;
			init.ALMB.Minutes = 0;
			init.ALMB.Seconds = 20;
			init.ALMB.Day = 0;
			init.ALMB.WeekDaySel = AlarmValue::WeekDay_Disable;

			init.ALMB.Mask = LL_RTC_ALMB_MASK_DATEWEEKDAY | LL_RTC_ALMB_MASK_HOURS
								|LL_RTC_ALMB_MASK_MINUTES;
		}

		ret = rtc.SetAlarm(&init);
	}

	return ret;
}






