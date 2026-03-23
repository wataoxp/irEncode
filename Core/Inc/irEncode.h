/*
 * irEncode.h
 *
 *  Created on: Mar 8, 2026
 *      Author: wataoxp
 */

#ifndef INC_IRENCODE_H_
#define INC_IRENCODE_H_

#include "irCommon.h"

namespace irEncodeUnits{
	typedef struct{
		uint16_t Period;
		uint16_t DummyRCR;
		uint16_t Duty;
	}EncodePulseData;

	typedef uint32_t EncodeByteData;
}

class irEncode{
private:
	irEncodeUnits::EncodePulseData* EncodeBuffer;	// エンコード後のパルスデータ

	void EncodeSetSignal(uint32_t number,uint16_t Period,uint16_t Duty);
	uint32_t EncodeNEC(uint32_t data,uint32_t num);
	uint32_t EncodeAEHA(uint32_t*,uint32_t num);
public:
	irEncode(irEncodeUnits::EncodePulseData* address);
	uint32_t Encode(irEncodeUnits::EncodeByteData*,Infrared::FormatSymbol type,uint32_t num);
	uint32_t SetEncodeAddress(irEncodeUnits::EncodePulseData* buffer);
	irEncodeUnits::EncodePulseData* GetEncodeAddress(void);
};

/* パルスデータのバッファアドレスを返す */
inline irEncodeUnits::EncodePulseData* irEncode::GetEncodeAddress(void)
{
	return EncodeBuffer;
}



#endif /* INC_IRENCODE_H_ */
