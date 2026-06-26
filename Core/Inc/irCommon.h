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

	constexpr uint32_t BurstTransferLength = 3;
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

	typedef enum{
		LeaderBit = 1,
		DataBit = 32,		// AEHAは可変長なので32ビットを1つの区切りとする
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

namespace DeviceCommands{

	// ELSONIC CeilingLight
	namespace ED505{
		constexpr uint32_t GetLeaderCode() { return 0x01D00C30; }
		constexpr uint32_t GetDataSize() { return 40U;}
		constexpr uint32_t GetDataShift() { return 24U; }	// データ部は左シフトする

		enum Data{
			FullLight 	= 0x09,
			NightLight 	= 0x11,
			Up			= 0x02,
			Down		= 0x22,
			Power		= 0x03,
			HerfLight	= 0x24,
			Memory		= 0x65,
			Timer		= 0x24,
		};
	}
}

static inline uint32_t ByteSwap(uint32_t Binary)
{
	uint32_t msb = 0;

	msb |= (Binary & 0xFF) << 24;
	msb |= ((Binary >> 8) & 0xFF) << 16;
	msb |= ((Binary >> 16) & 0xFF) << 8;
	msb |= (Binary >> 24) & 0xFF;

	return msb;
}

#endif /* INC_IRCOMMON_H_ */
