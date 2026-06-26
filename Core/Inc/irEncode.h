/*
 * irEncode.h
 *
 *  Created on: Mar 8, 2026
 *      Author: wataoxp
 */

#ifndef INC_IRENCODE_H_
#define INC_IRENCODE_H_

#include "irCommon.h"
#include <stddef.h>
#include <cassert>

namespace irEncodeUnits{
	typedef struct{
		uint16_t Period;
		uint16_t DummyRCR;
		uint16_t Duty;
	}EncodePulseData;					// エンコード後のデータ

	typedef uint32_t EncodeByteData;	// エンコード前のデータ

	enum ReturnCode{
		Success,
		OverSize,
		ErrorSize,
	};
}

class irEncode{
private:
	irEncodeUnits::EncodePulseData* EncodeBuffer;

	void EncodeSetSignal(uint32_t number,uint16_t Period,uint16_t Duty);
	uint32_t EncodeNEC(uint32_t data);
	uint32_t EncodeAEHA(uint32_t* buf,uint32_t DataBits);
public:
	irEncode(irEncodeUnits::EncodePulseData* address);
	uint32_t Encode(irEncodeUnits::EncodeByteData*,Infrared::FormatSymbol type,uint32_t num);
};

// irEncode用管理クラス
template <size_t BufferSize>
class irEncodeGate{
private:
	irEncodeUnits::EncodePulseData Buffer[BufferSize] = {};
	irEncode Tx;
	uint32_t BitNum = 0;			// エンコード前のバイトデータ数
	uint32_t PulseLength = 0;		// エンコード後のパルスデータ数
public:
	// エンコードデータバッファを渡す
	irEncodeGate() :Tx(Buffer)
	{
		;
	}

	// データビットのみを指定
	inline void SetPulseLength(uint32_t num)
	{
		BitNum = num;
	}

	inline irEncodeUnits::EncodePulseData* GetPulseAddress()
	{
		return Buffer;
	}

	inline uint32_t GetPulseLength() const
	{
		return PulseLength;
	}

	inline uint32_t GetBurstLength() const
	{
		return PulseLength * Infrared::BurstTransferLength;
	}

	irEncodeUnits::ReturnCode initEncodeData(irEncodeUnits::EncodeByteData* data,Infrared::FormatSymbol type)
	{
		using namespace Infrared;
		using namespace irEncodeUnits;

		uint32_t RequiredPulseLength = 0;

		if(type == FormatSymbol::NEC)
		{
			RequiredPulseLength = FormatNEC::TotalSize;
		}
		else if(type == FormatSymbol::AEHA)
		{
			if(BitNum < FormatAEHA::DataBit)
			{
				assert(false);
				return ErrorSize;
			}

			RequiredPulseLength = BitNum + FormatAEHA::LeaderBit + FormatAEHA::StopBit + FormatAEHA::IdleBit;
		}

		if(RequiredPulseLength > BufferSize)
		{
			assert(false);
			return OverSize;
		}

		PulseLength = Tx.Encode(data,type,BitNum);

		return Success;
	}
};



#endif /* INC_IRENCODE_H_ */
