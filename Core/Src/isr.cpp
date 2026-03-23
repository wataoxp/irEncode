/*
 * isr.cpp
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */


#include "isr.h"
#include "config.h"		// WatchClock

static __IO ISR_Flags Flag = {0};
static __IO uint32_t mTick = 0;

/* Getter/Setter */

const ISR_Flags* GetFlagStruct(void)
{
	return (const ISR_Flags*)&Flag;
}

void SetExtiFlag(bool flag)
{
	Flag.ExtiFlag = flag;
}

void SetWutFlag(bool flag)
{
	Flag.WutFlag = flag;
}

void SetALMAFlag(bool flag)
{
	Flag.AlmaFlag = flag;
}

void SetALMBFlag(bool flag)
{
	Flag.AlmbFlag = flag;
}


/* Interrupt Service Routine */

extern "C"{

void SysTick_Handler(void)
{
	using namespace DebouceFilter;
	static  __IO uint8_t DebounceCount = 0;

	mTick++;
	if((mTick & CheckInterval) == 0)
	{
		// 現在のピン状態を8ビットに格納(シフトして古いビットは捨てる)
		DebounceCount = (DebounceCount << 1) | LL_GPIO_IsInputPinSet(GPIOB, 1 << WatchClock::EncoderPushPin);

		if(DebounceCount == EnableCount)
		{
			Flag.ExtiFlag = true;
		}

		if(DebounceCount == ClearCount)
		{
			LL_EXTI_ClearFallingFlag_0_31(WatchClock::EncoderPushLine);
			LL_EXTI_EnableIT_0_31(WatchClock::EncoderPushLine);
		}
	}
}

void EXTI4_15_IRQHandler(void)
{

	if(LL_EXTI_IsActiveFallingFlag_0_31(WatchClock::EncoderPushLine))
	{
		LL_EXTI_ClearFallingFlag_0_31(WatchClock::EncoderPushLine);
		LL_EXTI_DisableIT_0_31(WatchClock::EncoderPushLine);
	}
}

void TIM3_TIM4_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_CC1(WatchClock::EncoderTimer))
	{
		LL_TIM_ClearFlag_CC1(WatchClock::EncoderTimer);
	}
	if(LL_TIM_IsActiveFlag_CC2(WatchClock::EncoderTimer))
	{
		LL_TIM_ClearFlag_CC2(WatchClock::EncoderTimer);
	}
}
void RTC_TAMP_IRQHandler(void)
{
	if(LL_RTC_IsActiveFlag_ALRA(RTC))
	{
		LL_RTC_ClearFlag_ALRA(RTC);
		Flag.AlmaFlag = true;
	}

	if(LL_RTC_IsActiveFlag_ALRB(RTC))
	{
		LL_RTC_ClearFlag_ALRB(RTC);
		Flag.AlmbFlag = true;
	}

	if(LL_RTC_IsActiveFlag_WUT(RTC))
	{
		LL_RTC_ClearFlag_WUT(RTC);
		Flag.WutFlag = true;
	}
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

