/*
 * config.h
 *
 *  Created on: Feb 21, 2026
 *      Author: wataoxp
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "TM1652.h"
#include "encoder.h"
#include "irCommon.h"
#include "irEncode.h"
#include "watchclock.h"

class Config{
private:
	RealClock& rtc;
public:
	Config(RealClock& RTCx);

	uint32_t WatchSetUp(UART& uart,CoreClock source,TM1652& seg);
	uint32_t SetDate();
	uint32_t ConfigAlarm(RealClockSpace::Options alarm,RealClockSpace::Options sel);

	uint32_t EncoderSetUp(TIM& tim,Encoder& encode);
	uint32_t ButtonSetUp(void);

	uint32_t LowFrequencyInit(TIM& lf);
	uint32_t HighFrequencyInit(TIM& hf);
	uint32_t irEncodeInit(TIM_TypeDef* LfTim,TIM& lf,TIM& hf,DMA& dma,irEncode& ir);

	void EnterSleepMode(TIM& encoder);
	void ExitSleepMode(TIM& encoder);
};

inline void Config::EnterSleepMode(TIM& encoder)
{
	using namespace WatchClock;

	encoder.DisablePulse(LL_TIM_CHANNEL_CH1);
	encoder.DisablePulse(LL_TIM_CHANNEL_CH2);
	encoder.DisableTimer();

	LL_SYSTICK_DisableIT();
	LL_EXTI_ClearFallingFlag_0_31(ExtiPin::GetAllExtiLine());
	LL_EXTI_EnableFallingTrig_0_31(ExtiPin::GetAllExtiLine());
	GPIO_CLEAR(Indicator::GPIOx,Indicator::IndicatorPos);
}

inline void Config::ExitSleepMode(TIM& encoder)
{
	using namespace WatchClock;

	GPIO_WRITE(Indicator::GPIOx,Indicator::IndicatorPos);
	LL_SYSTICK_EnableIT();
	LL_EXTI_ClearFallingFlag_0_31(ExtiPin::GetAllExtiLine());
	LL_EXTI_DisableFallingTrig_0_31(ExtiPin::GetAllExtiLine());

	encoder.EnablePulse(LL_TIM_CHANNEL_CH1);
	encoder.EnablePulse(LL_TIM_CHANNEL_CH2);
	encoder.EnableTimer();
}

#endif /* INC_CONFIG_H_ */
