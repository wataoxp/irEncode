/*
 * isr.h
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */

#ifndef INC_ISR_H_
#define INC_ISR_H_

namespace DebouceFilter{
	typedef enum{
		CheckInterval = (16-1),		// 16で剰余。16msずつチェック
		EnableCount = 1 << 7,		// Highの後に7回連続のLowで押下と判断
		ClearCount = 0xFF,			// 8回すべてHighなら離されたと判断
	}FileterVal;
}

typedef struct{
	volatile bool AlmaFlag;
	volatile bool AlmbFlag;
	volatile bool WutFlag;
	volatile bool ExtiFlag;
}ISR_Flags;

const ISR_Flags* GetFlagStruct(void);


void SetExtiFlag(bool);
void SetWutFlag(bool);
void SetALMAFlag(bool);
void SetALMAFlag(bool);

extern "C" {

void SysTick_Handler(void);
void EXTI4_15_IRQHandler(void);
void TIM3_TIM4_IRQHandler(void);
void RTC_TAMP_IRQHandler(void);

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);

}



#endif /* INC_ISR_H_ */
