/*
 * irDecode.h
 *
 *  Created on: Mar 8, 2026
 *      Author: wataoxp
 */

#ifndef INC_IRDECODE_H_
#define INC_IRDECODE_H_

#include "irCommon.h"

namespace irDecodeUnits{
	constexpr uint32_t DecodeBufLength = 4;
}

class irDecode{
private:
	const uint16_t *PulseDMABuffer;							// DMA受信バッファを管理
	uint32_t DecodeBuffer[irDecodeUnits::DecodeBufLength];		// デコード後のバイトデータ

	uint32_t DecodeNEC(const uint16_t*Address);
	uint32_t DecodeAEHA(const uint16_t*Address);
public:
	irDecode(const uint16_t* address);
	uint32_t Decode(void);

	void SetDecodeBit(uint32_t count);
	uint32_t GetDecodeBit(void);
	uint32_t SetDecodeAddress(uint16_t *buffer);
	const uint16_t* GetDecodeAddress(void);
	uint32_t GetDecodeWord(uint8_t count);
};

inline const uint16_t* irDecode::GetDecodeAddress(void)
{
	return PulseDMABuffer;
}

inline uint32_t irDecode::GetDecodeWord(uint8_t count)
{
	if(count > irDecodeUnits::DecodeBufLength-1)
	{
		return UINT32_MAX;
	}
	return DecodeBuffer[count];
}



#endif /* INC_IRDECODE_H_ */
