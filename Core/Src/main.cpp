#include "mylib.h"
#include "config.h"
#include "clockview.h"
#include "state.h"
#include "isr.h"
#include "irEncode.h"

#include "watchclock.h"

using namespace WatchClock;
using namespace SendIR;

void TransferIR(TIM& hf,TIM& lf,DMA& dma,uint32_t Length);
uint32_t irEncodeInit(TIM_TypeDef* LfTim,TIM& lf,TIM& hf,DMA& dma,irEncodeUnits::EncodePulseData* Address);
uint32_t HighFrequencyInit(TIM& hf);
uint32_t LowFrequencyInit(TIM& lf);

int main(void)
{
	constexpr CoreClock ClockSource = HSICLOCK;

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

	RCC_Config(ClockSource, rccStatus::InternalClock);

	DelaySource Delay(ClockSource);

	TIM LF(LfTimer::Timer);
	TIM HF(HfTimer::Timer);
	DMA dma(MemoryAccess::Handle);

	irEncodeUnits::EncodeByteData TxData[2] = {
			DeviceCommands::ED505::GetLeaderCode(),
			DeviceCommands::ED505::Power << DeviceCommands::ED505::GetDataShift(),
	};

	irEncodeGate<48> irGate;
	irGate.SetPulseLength(40);		// データビットのみ
	irGate.initEncodeData(TxData, Infrared::FormatSymbol::AEHA);
	irEncodeInit(LfTimer::Timer, LF, HF, dma, irGate.GetPulseAddress());

	while(1)
	{
		Delay.mDelay(500);
		TransferIR(HF, LF, dma, irGate.GetBurstLength());
		Delay.mDelay(500);
		TransferIR(HF, LF, dma, irGate.GetBurstLength());
	}
}

uint32_t irEncodeInit(TIM_TypeDef* LfTim,TIM& lf,TIM& hf,DMA& dma,irEncodeUnits::EncodePulseData* Address)
{
	using namespace WatchClock::SendIR::MemoryAccess;
	uint32_t ret = 0;

	ret += HighFrequencyInit(hf);
	ret += LowFrequencyInit(lf);

	ret += lf.ConfigDMA(Channel, LL_TIM_CCDMAREQUEST_UPDATE);
	ret += DMA_Config(dma, Channel, LL_DMAMUX_REQ_TIM16_UP, (uint32_t*)Address, (uint32_t*)&LfTim->DMAR);

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

uint32_t LowFrequencyInit(TIM& lf)
{
	using namespace WatchClock::SendIR::LfTimer;

	TIM_Config(lf, Prescaler, Period);
#ifdef DEBUG
	return PWM_Config(lf, GPIOx, PinPos, Alternate, Channel, LL_TIM_OCMODE_PWM1);
#else
	return lf.ConfigPWM(Channel, LL_TIM_OCMODE_PWM1);
#endif
}

uint32_t HighFrequencyInit(TIM& hf)
{
	using namespace WatchClock::SendIR::HfTimer;

	TIM_Config(hf, Prescaler, Period);
	hf.SetCH1CompareValue(Period/Duty);

	return PWM_Config(hf, GPIOx, PinPos, Alternate, Channel, LL_TIM_OCMODE_PWM1);
}

void TransferIR(TIM& hf,TIM& lf,DMA& dma,uint32_t Length)
{
	using namespace WatchClock::SendIR;

	hf.EnableTimer();
	hf.EnablePulse(HfTimer::Channel);
	lf.EnableTimer();
	lf.EnablePulse(LfTimer::Channel);

	LL_DMA_ClearFlag_TC2(MemoryAccess::Handle);
	dma.StartDMA(MemoryAccess::Channel, Length);
	dma.StopDMAisChannel2();

	LL_TIM_ClearFlag_UPDATE(LfTimer::Timer);
	while(LL_TIM_IsActiveFlag_UPDATE(LfTimer::Timer) == 0);
	LL_TIM_ClearFlag_UPDATE(LfTimer::Timer);

	// 次の送信までは無効化
	hf.DisablePulse(HfTimer::Channel);
	hf.DisableTimer();
	lf.DisablePulse(LfTimer::Channel);
	lf.DisableTimer();
}
