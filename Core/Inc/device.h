/*
 * device.h
 *
 *  Created on: Apr 5, 2026
 *      Author: wataoxp
 */

#ifndef INC_DEVICE_H_
#define INC_DEVICE_H_

#include <clockview.h>

namespace DeviceUnits{
	// リーダー+データの総計が41ビット。ストップとアイドルで43ビット
	constexpr uint32_t CeilingLightControlBitNum = 41;

	// 信号データ
	constexpr uint32_t CeilingLightHighByte = 0x01D00C30;
	constexpr uint8_t CeilingLightPowerButton = 0x03;
	constexpr uint8_t CeilingLightFullLight = 0x09;
	constexpr uint8_t CeilingLightHerfLight = 0x24;

	constexpr uint32_t CeilingLightLowBytePos = 24;
}

class Device{
private:
	uint32_t BitLength;
	irEncodeUnits::EncodeByteData TxData[2];

	uint8_t MelodyNum;
public:
	Device();
	void TransferIR(TIM& hf,TIM& lf,DMA& dma,irEncode& Tx);
	void RingMelody(M8058S& melody);
};



#endif /* INC_DEVICE_H_ */
