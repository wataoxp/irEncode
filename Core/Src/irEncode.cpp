/*
 * irEncode.cpp
 *
 *  Created on: Mar 8, 2026
 *      Author: wataoxp
 */

#include "irEncode.h"
#include <cassert>

using namespace Infrared;
using namespace irEncodeUnits;

irEncode::irEncode(EncodePulseData* address)
{
	assert(address != nullptr);
	EncodeBuffer = address;
}

inline void irEncode::EncodeSetSignal(uint32_t number,uint16_t Period,uint16_t Duty)
{
	EncodeBuffer[number].Period = Period;
	EncodeBuffer[number].Duty = Duty;
}

uint32_t irEncode::Encode(uint32_t* data,FormatSymbol type,uint32_t num)
{
	uint32_t ret = 0;

	if(type == FormatSymbol::NEC)
	{
		ret = EncodeNEC(data[0]);
	}
	else if(type == FormatSymbol::AEHA)
	{
		ret = EncodeAEHA(data,num);
	}

	return ret;
}

uint32_t irEncode::EncodeNEC(uint32_t data)
{
	using namespace FormatNEC;

	uint32_t bit = 0;
	uint32_t count = 0;

	uint32_t msb = ByteSwap(data);

	EncodeSetSignal(count++, LeaderCodePeriod, LeaderCodeDuty);

	for(; count < DataBitNum;count++)
	{
		EncodeBuffer[count].Period = (msb & (1 << bit++))? BitHigh:BitLow;
		EncodeBuffer[count].Duty = BitDuty;
	}

	EncodeSetSignal(count++, StopPeriod, StopDuty);
	EncodeSetSignal(count++, IdlePeriod, IdleDuty);

	return count;
}


// データ数のみを受け取り、最終的な送信データ数を返す
uint32_t irEncode::EncodeAEHA(uint32_t* buf,uint32_t DataBits)
{
	using namespace FormatAEHA;

	uint32_t bit = 0;
	uint32_t count = 0;		// 信号の総データ数
	uint32_t idx = 0;

	uint32_t msb = ByteSwap(buf[idx]);

	EncodeSetSignal(count++, LeaderCodePeriod, LeaderCodeDuty);

	// データ部を作成する
	for(uint32_t i = 0;i < DataBits;i++)
	{
		EncodeBuffer[count].Period = (msb & (1 << bit++))? BitHigh:BitLow;
		EncodeBuffer[count].Duty = BitDuty;

		if(bit % DataBit == 0)	// 32ビット解析後次のデータへ
		{
			idx++;
			msb = ByteSwap(buf[idx]);
			bit = 0;
		}
		count++;
	}

	EncodeSetSignal(count++, StopPeriod, StopDuty);
	EncodeSetSignal(count++, IdlePeriod, IdleDuty);

	return count;
}

#if 0
uint32_t irEncode::EncodeAEHA(uint32_t* buf,uint32_t num)
{
	using namespace FormatAEHA;

	uint32_t bit = 0;
	uint32_t count = 0;
	uint32_t idxLimit = (num - (LeaderBit + StopBit + IdleBit)) / MinDataBit;	// データ部の数をチェック
	uint32_t idx = 0;

	uint32_t msb = ByteSwap(buf[idx]);

	EncodeSetSignal(count++, LeaderCodePeriod, LeaderCodeDuty);

	for(; count < num;count++)
	{
		EncodeBuffer[count].Period = (msb & (1 << bit++))? BitHigh:BitLow;
		EncodeBuffer[count].Duty = BitDuty;

		if(bit % MinDataBit == 0)	// bitはインクリメント後に評価
		{
			idx = (idx < idxLimit)? idx+1:0;
			msb = ByteSwap(buf[idx]);
			bit = 0;
		}
	}

	EncodeSetSignal(count++, StopPeriod, StopDuty);
	EncodeSetSignal(count++, IdlePeriod, IdleDuty);

	return count;
}
#endif


