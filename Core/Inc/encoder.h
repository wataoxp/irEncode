/*
 * encoder.h
 *
 *  Created on: Feb 17, 2026
 *      Author: wataoxp
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "periph.h"
#include "tim.h"

namespace EncoderUnits{
	constexpr uint32_t AbsoluteValueisMode3 = 4;
}

class Encoder{
private:
	TIM& timer;
	TIM_TypeDef *TIMx;
public:
	Encoder(TIM& tim,TIM_TypeDef *Tim);
	uint32_t PinInit(GPIO_TypeDef* A_Port,uint32_t A_Pin,uint32_t A_AF,GPIO_TypeDef* B_Port,uint32_t B_Pin,uint32_t B_Af);
	void Init(uint32_t Division,uint32_t Filter,uint32_t Mode);
	int16_t GetCount(void);
	bool CheckValueMode3(void);
};





#endif /* INC_ENCODER_H_ */
