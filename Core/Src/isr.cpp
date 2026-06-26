/*
 * isr.cpp
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */


#include "isr.h"
#include "debounce.h"
#include "watchclock.h"

using namespace WatchClock;

// __IOを除いたことで最適化の影響を受けるかもしれない
static isrFlags iFlag;

/* Getter/Setter */
const isrFlags& GetISRStruct(void)
{
	return (const isrFlags&)iFlag;
}

void ResetExtCommand(void)	// もし割り込みとの競合が発生するならここで割り込みを一時停止する
{
	iFlag.ExtCommand = 0;
}

void ResetIntCommand(void)
{
	iFlag.IntCommand = 0;
}

void ResetWutFlag(void)
{
	iFlag.IntCommand &= ~Parameter::WutFlagVal;
}

void ResetAlmaFlag(void)
{
	iFlag.IntCommand &= ~Parameter::AlmaFlagVal;

}

void ResetAlmbFlag(void)
{
	iFlag.IntCommand &= ~Parameter::AlmbFlagVal;
}

/* Interrupt Service Routine */

extern "C"{

void SysTick_Handler(void)
{
	static __IO uint32_t mTick;

	if(mTick++ < Debounce::GetInterval())
	{
		return;
	}
	mTick = 0;
	Debounce::ButtonFilter(iFlag.ExtCommand);
}

void RTC_TAMP_IRQHandler(void)
{
	if(LL_RTC_IsActiveFlag_ALRA(RTC))
	{
		LL_RTC_ClearFlag_ALRA(RTC);
		iFlag.IntCommand |= Parameter::AlmaFlagVal;
	}

	if(LL_RTC_IsActiveFlag_ALRB(RTC))
	{
		LL_RTC_ClearFlag_ALRB(RTC);
		iFlag.IntCommand |= Parameter::AlmbFlagVal;
	}

	if(LL_RTC_IsActiveFlag_WUT(RTC))
	{
		LL_RTC_ClearFlag_WUT(RTC);
		iFlag.IntCommand |= Parameter::WutFlagVal;
	}
}

void TIM3_TIM4_IRQHandler(void)
{
	using namespace RotaryEncoder;
}

void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

}// extern "C"
