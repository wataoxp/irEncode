/*
 * config.h
 *
 *  Created on: Feb 21, 2026
 *      Author: wataoxp
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "mylib.h"
#include "TM1652.h"
#include "so1602.h"
#include "encoder.h"

namespace WatchClock{
	// 型名の後のconstについて
	// LEDPortの中身は操作できるが、指す対象を変えることは不可能
	GPIO_TypeDef* const LEDPort = GPIOA;
	constexpr uint32_t LEDPin = Pin8;

	// TM1652
	USART_TypeDef* const SegSerial = USART1;
	GPIO_TypeDef* const SegPort = GPIOB;
	constexpr uint32_t TxPin = Pin6;
	constexpr uint32_t RxPin = Pin7;
	constexpr uint32_t TxAf = LL_GPIO_AF_0;
	constexpr uint32_t RxAf = LL_GPIO_AF_0;

	// SO1602
	I2C_TypeDef* const SO1602Wire = I2C1;
	GPIO_TypeDef* const SO1602Port = GPIOA;
	constexpr uint32_t SCLPin = Pin9;
	constexpr uint32_t SDAPin = Pin10;
	constexpr uint32_t SCLAf = LL_GPIO_AF_6;
	constexpr uint32_t SDAAf = LL_GPIO_AF_6;

	// Encoder
	TIM_TypeDef* const EncoderTimer = TIM3;
	GPIO_TypeDef* const EncoderPort = GPIOA;
	constexpr uint32_t ChannelA_Pin = Pin6;
	constexpr uint32_t ChannelB_Pin = Pin7;
	constexpr uint32_t ChannelA_Af = LL_GPIO_AF_1;
	constexpr uint32_t ChannelB_Af = LL_GPIO_AF_1;

	// EXTI(Encoder)
	GPIO_TypeDef* const EncoderPushPort = GPIOB;
	constexpr uint32_t EncoderPushPin = Pin14;
	constexpr uint32_t EncoderPushLine = LL_EXTI_LINE_14;
}

uint32_t WatchSetUp(RealClock& rtc,UART& uart,CoreClock source,TM1652& seg);
uint32_t EncoderSetUp(TIM& tim,Encoder& encode);
uint32_t ButtonSetUp(void);
void ConfigLCD(I2C& i2c,CoreClock source,SO1602& lcd);
uint32_t ConfigDate(RealClock& rtc);
uint32_t ConfigAlarm(RealClock& rtc,RealClockSpace::Options alarm,RealClockSpace::Options sel);

#endif /* INC_CONFIG_H_ */
