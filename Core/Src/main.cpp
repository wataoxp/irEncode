#include <irCommon.h>
#include "irEncode.h"
#include "mylib.h"

namespace TxIR{

constexpr uint32_t txChannel = LL_DMA_CHANNEL_2;

constexpr uint32_t lfChannel = LL_TIM_CHANNEL_CH1;

constexpr uint32_t hfChannel = LL_TIM_CHANNEL_CH1;

//constexpr uint32_t hf_Period = 1650;	//64M/1650 = 38.78kHz
constexpr uint32_t hf_Period = 413;	// 上記の左辺を4で割る

constexpr uint32_t hf_Duty = 3;			//デューティー比30%

//constexpr uint32_t LowTimerPsc = 64;
constexpr uint32_t LowTimerPsc = 16;
constexpr uint32_t HighTimerPsc = 1;
}

static irEncodeUnits::EncodePulseData TxData[64];

uint32_t IR_HighFrequencyInit(TIM& hf,GPIO_TypeDef *GPIOx,uint32_t Pin,uint32_t Alternate);
uint32_t IR_LowFrequencyInit(TIM& lf,GPIO_TypeDef *GPIOx,uint32_t Pin,uint32_t Alternate);
uint32_t IR_TransmitInit(DMA_TypeDef *DMAx,TIM_TypeDef *TIMx,DMA& dma,TIM& tim,irEncode& ir);

int main(void)
{
	constexpr CoreClock ClockSource = HSICLOCK;

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

	RCC_Config(ClockSource, rccStatus::InternalClock);

	using Delay = DelayMode::Standard;
	Delay::Init(ClockSource);

	TIM LF(TIM16);
	TIM HF(TIM17);
	DMA dma(DMA1);
	irEncode TxIR(TxData);
	uint32_t length = 0;
	uint32_t data[4];

	IR_HighFrequencyInit(HF, GPIOB, Pin9, LL_GPIO_AF_0);		//IR_OUT
	IR_LowFrequencyInit(LF, GPIOB, Pin8, LL_GPIO_AF_2);
	IR_TransmitInit(DMA1, TIM16, dma, LF, TxIR);

	HF.SetCH1CompareValue(TxIR::hf_Period/TxIR::hf_Duty);
	HF.EnableTimer();
	HF.EnablePulse(TxIR::hfChannel);

	LF.EnableTimer();
	LF.EnablePulse(TxIR::lfChannel);

	LL_DMA_EnableIT_TC(DMA1, TxIR::txChannel);
	__NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	__NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);

	GPIO_Config(GPIOA, Pin7, LL_GPIO_MODE_INPUT);

	uint32_t bit = 0;

	while(1)
	{
		do{
			bit = LL_GPIO_IsInputPinSet(GPIOA, 1 << Pin7);
		}while(bit != 0);

		data[0] = 0x01D00C30;
		data[1] = 0x03000000;

		length = TxIR.Encode(data, Infrared::FormatSymbol::AEHA,FormatAEHA::CeilingLightControlBitNum);

//		data[0] = 0x806300FF;
//		length = TxIR.Encode(data, Infrared::FormatSymbol::NEC, 0);

		length *= Infrared::BurstTransferLength;
		dma.StartDMA(TxIR::txChannel, length);

		Delay::mDelay(200);
		dma.StopDMA(TxIR::txChannel);
	}
}

extern "C"{
void DMA1_Channel2_3_IRQHandler(void)
{
	if(LL_DMA_IsActiveFlag_TC2(DMA1) != 0)
	{
		LL_DMA_ClearFlag_TC2(DMA1);
	}
}
}

uint32_t IR_HighFrequencyInit(TIM& hf,GPIO_TypeDef *GPIOx,uint32_t Pin,uint32_t Alternate)
{
	uint32_t ret = 0;

	TIM_Config(hf, TxIR::HighTimerPsc, TxIR::hf_Period);
	ret += PWM_Config(hf, GPIOx, Pin, Alternate, TxIR::hfChannel, LL_TIM_OCMODE_PWM1);

	return ret;
}

uint32_t IR_LowFrequencyInit(TIM& lf,GPIO_TypeDef *GPIOx,uint32_t Pin,uint32_t Alternate)
{
	uint32_t ret = 0;

	TIM_Config(lf, TxIR::LowTimerPsc, 1000);
	ret += PWM_Config(lf, GPIOx, Pin, Alternate, TxIR::lfChannel, LL_TIM_OCMODE_PWM1);

	return ret;
}

uint32_t IR_TransmitInit(DMA_TypeDef *DMAx,TIM_TypeDef *TIMx,DMA& dma,TIM& tim,irEncode& ir)
{
	uint32_t ret = 0;

	ret += tim.ConfigDMA(TxIR::lfChannel, LL_TIM_CCDMAREQUEST_UPDATE);
	ret += DMA_Config(dma, TxIR::txChannel, LL_DMAMUX_REQ_TIM16_UP, (uint32_t*)ir.GetEncodeAddress(), (uint32_t*)&TIMx->DMAR);
	LL_DMA_SetDataTransferDirection(DMAx, TxIR::txChannel, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetMemorySize(DMAx, TxIR::txChannel, LL_DMA_MDATAALIGN_HALFWORD);
	LL_TIM_ConfigDMABurst(TIMx, LL_TIM_DMABURST_BASEADDR_ARR, LL_TIM_DMABURST_LENGTH_3TRANSFERS);

	LL_SYSCFG_SetIRModEnvelopeSignal(LL_SYSCFG_IR_MOD_TIM16);
	LL_SYSCFG_SetIRPolarity(LL_SYSCFG_IR_POL_INVERTED);		//TIM出力とのXORを取っているだけ、アイドル時Low

	return ret;
}
