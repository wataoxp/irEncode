/*
 * ir.h
 *
 *  Created on: Jun 30, 2025
 *      Author: wataoxp
 */

#ifndef INC_IRCOMMON_H_
#define INC_IRCOMMON_H_

#include "periph.h"

namespace Infrared{
	constexpr uint32_t DecodeBufLength = 4;

	enum class FormatSymbol{
		NEC,
		AEHA,
	};
}

namespace FormatNEC{
	typedef enum{
		LeaderBit = 1,
		DataBit = 32,
		StopBit = 1,
		IdleBit = 1,		//ir.Encode関数でアイドル時の極性を確定させるため
	}BufferSize;

	typedef enum{
		Turn = 500,
		LeaderCode = Turn * 24,
		RepeatCode = Turn * 20,
		RepeatThreshold = 2000,
		LowPeriod = Turn * 2,
		HighPeriod = Turn * 3,
	}DecodeTiming;

	constexpr uint32_t DataBitNum = LeaderBit + DataBit;
	constexpr uint32_t TotalSize = DataBitNum + StopBit + IdleBit;
//	constexpr uint32_t NecDMAlength = NecTotalSize * 3;

	typedef enum{
		LeaderCodePeriod = 13500,
		LeaderCodeDuty = 9000,
		BitHigh = 2200,
		BitLow = 1100,
		BitDuty = 400,
		StopPeriod = BitLow,
		StopDuty = 500,
		IdlePeriod = 5000,
		IdleDuty = 0,
	}EncodeTiming;

	typedef enum{
		success,
		Failed,
	}NECreturnCode;
}

namespace FormatAEHA{

	/*** 2026-03-08 シーリングライト用に決め打ちする ***/
	// リーダー+データの総計が41ビット。ストップとアイドルで43ビット
	constexpr uint32_t CeilingLightControlBitNum = 41;

	typedef enum{
		LeaderBit = 1,
		MinDataBit = 32,	// AEHAは可変長なので最小値として設定
		StopBit = 1,
		IdleBit = 1,		// 最後の出力値を確定
	}BufferSize;

	typedef enum{
		Turn = 400,
		LeaderCode = Turn * 12,
		RepeatCode = Turn * 16,
		LowPeriod = Turn * 2,
		HighPeriod = Turn * 3,
	}DecodeTiming;

	constexpr uint32_t DataBitNum = LeaderBit + MinDataBit;
//	constexpr uint32_t AehaTotalsize = AehaDataBitNum + StopBit + IdleBit;
//	constexpr uint32_t AehaDMAlength = AehaTotalsize * 3;

	typedef enum{
		LeaderCodePeriod = 4900,
		LeaderCodeDuty = 3400,
		BitHigh = 1650,
		BitLow = 850,
		BitDuty = 500,
		StopPeriod = BitLow,
		StopDuty = 500,
		IdlePeriod = 5000,
		IdleDuty = 0,
	}EncodeTiming;
}

static inline uint32_t ByteSwap(uint32_t Binary)
{
	uint32_t msb = 0;

	msb |= (Binary & 0xFF) << 24;
	msb |= ((Binary >> 8) & 0xFF) << 16;
	msb |= ((Binary >> 16) & 0xFF) << 8;
	msb |= (Binary >> 24) & 0xFF;

	return msb;
//	return ((Binary & 0xFF) << 24) | (((Binary >> 8) & 0xFF) << 16) | (((Binary >> 16) & 0xFF) << 8) | (((Binary >> 24) & 0xFF));
}

#endif /* INC_IRCOMMON_H_ */
