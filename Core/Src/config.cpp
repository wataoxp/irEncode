/*
 * config.cpp
 *
 *  Created on: Feb 26, 2026
 *      Author: wataoxp
 */

#include "config.h"

Config::Config(RealClock& RTCx) : rtc(RTCx)
{
	;
}

uint32_t Config::WatchSetUp(UART& uart,CoreClock source,TM1652& seg)
{
	using namespace WatchClock;
	using namespace SegSerial;

	uint32_t ret = 0;

	// TM1652通信用
	UART_Config(uart, source, GPIOx, TxPos, TxAf, GPIOx, RxPos, RxAf);

	seg.Clear();
	seg.SetBrightness(Parameter::DefaultBrightness());
	seg.WriteDig(0,0,0,0);

	// STOP1モードを有効化
	LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
	LL_LPM_EnableDeepSleep();

	// RTCの設定および割り込みの有効化
	ret = RTC_Config(rtc);
	__NVIC_SetPriority(RTC_TAMP_IRQn, 0);
	__NVIC_EnableIRQ(RTC_TAMP_IRQn);

	// RTC割りこみはEXTIライン19 立ち上がりエッジ固定
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_19);

	rtc.SetWakeUpTimer(Parameter::WakeUpCount());
	rtc.SetTime(LL_RTC_TIME_FORMAT_AM_OR_24, 0, 0, 0);

	return ret;
}

uint32_t Config::SetDate()
{
	using namespace RealClockSpace;
	DateConfig init;

	init.WeekDay = LL_RTC_WEEKDAY_TUESDAY;
	init.Month = LL_RTC_MONTH_FEBRUARY;
	init.Day = 3;
	init.Year = 26;

	return rtc.SetDate(init.WeekDay, init.Month, init.Day, init.Year);
}

uint32_t Config::EncoderSetUp(TIM& tim,Encoder& encode)
{
	using namespace WatchClock::RotaryEncoder;

	uint32_t ret = 0;

	TIM_Config(tim, 0, UINT16_MAX);

	// もっとも厳しいフィルタ設定
	encode.Init(LL_TIM_CLOCKDIVISION_DIV4, LL_TIM_IC_FILTER_FDIV32_N8, LL_TIM_ENCODERMODE_X4_TI12);
	ret = encode.PinInit(GPIOx, PhaseAPos, AfPhaseA, GPIOx, PhaseBPos, AfPhaseB);

	tim.EnableTimer();

	return ret;
}

uint32_t Config::ButtonSetUp(void)
{
	using namespace WatchClock::ExtiPin;
	uint32_t ret = 0;
	uint32_t Pull = LL_GPIO_PULL_UP;
	uint32_t Mode = LL_EXTI_MODE_EVENT;
	uint32_t Trigger = LL_EXTI_TRIGGER_FALLING;

	ret += EXTI_Config(GPIOx, PushPos, Pull, Mode, Trigger);
	ret += EXTI_Config(GPIOx, TimePos, Pull, Mode, Trigger);
	ret += EXTI_Config(GPIOx, AlmaPos, Pull, Mode, Trigger);
	ret += EXTI_Config(GPIOx, AlmbPos, Pull, Mode, Trigger);
	ret += EXTI_Config(GPIOx, BrightnessPos, Pull, Mode, Trigger);

	return ret;
}

// LF（変調周波数）はGPIOの設定は不要
uint32_t Config::LowFrequencyInit(TIM& lf)
{
	using namespace WatchClock::SendIR::LfTimer;

	TIM_Config(lf, Prescaler, Period);
#ifdef DEBUG
	return PWM_Config(lf, GPIOx, PinPos, Alternate, Channel, LL_TIM_OCMODE_PWM1);
#else
	return lf.ConfigPWM(Channel, LL_TIM_OCMODE_PWM1);
#endif
}

uint32_t Config::HighFrequencyInit(TIM& hf)
{
	using namespace WatchClock::SendIR::HfTimer;

	TIM_Config(hf, Prescaler, Period);
	hf.SetCH1CompareValue(Period/Duty);

	return PWM_Config(hf, GPIOx, PinPos, Alternate, Channel, LL_TIM_OCMODE_PWM1);
}

uint32_t Config::irEncodeInit(TIM_TypeDef* LfTim,TIM& lf,TIM& hf,DMA& dma,irEncode& ir)
{
	using namespace WatchClock::SendIR::MemoryAccess;
	uint32_t ret = 0;

	ret += HighFrequencyInit(hf);
	ret += LowFrequencyInit(lf);

	ret += lf.ConfigDMA(Channel, LL_TIM_CCDMAREQUEST_UPDATE);
//	ret += DMA_Config(dma, Channel, LL_DMAMUX_REQ_TIM16_UP, (uint32_t*)ir.GetEncodeAddress(), (uint32_t*)&LfTim->DMAR);

	LL_DMA_SetDataTransferDirection(Handle, Channel, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetMemorySize(Handle, Channel, LL_DMA_MDATAALIGN_HALFWORD);
	LL_TIM_ConfigDMABurst(WatchClock::SendIR::LfTimer::Timer, LL_TIM_DMABURST_BASEADDR_ARR, LL_TIM_DMABURST_LENGTH_3TRANSFERS);

	LL_SYSCFG_SetIRModEnvelopeSignal(LL_SYSCFG_IR_MOD_TIM16);
	LL_SYSCFG_SetIRPolarity(LL_SYSCFG_IR_POL_INVERTED);		//TIM出力とのXOR

//	LL_DMA_EnableIT_TC(HandleDMA, txChannel);
//	__NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
//	__NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);

	return ret;
}
