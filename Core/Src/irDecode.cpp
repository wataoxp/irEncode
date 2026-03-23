/*
 * irDecode.cpp
 *
 *  Created on: Mar 8, 2026
 *      Author: wataoxp
 */

#include "irDecode.h"
#include <string.h>
#include <cassert>

using namespace Infrared;

irDecode::irDecode(const uint16_t* address)
{
	assert(address != nullptr);		// 条件が偽であるときにアサート
	PulseDMABuffer = address;
}

uint32_t irDecode::Decode(void)
{
	uint32_t DecodeCount = 0;

	//未満で判定するので余裕を持った値に

	if(PulseDMABuffer[1] < FormatAEHA::LeaderCode+200)
	{
		//リピートコードは無いのでデータ部分から渡す
		DecodeCount = DecodeAEHA(&PulseDMABuffer[2]);
	}
	else if(PulseDMABuffer[1] < FormatNEC::LeaderCode+2000)
	{
		//リピートコード判定用にリーダーコードも渡す
		DecodeCount = DecodeNEC(&PulseDMABuffer[1]);
	}

	return DecodeCount;
}

uint32_t irDecode::DecodeNEC(const uint16_t*Address)
{
	const uint16_t *PulseNEC = Address;

	uint32_t num = 0;
	uint32_t numBits = 0;

	DecodeBuffer[0] = 0;

	if((PulseNEC[num++] - FormatNEC::RepeatCode) < FormatNEC::RepeatThreshold)
	{
		DecodeBuffer[0] = UINT32_MAX;
		return 0;
	}

	while(PulseNEC[num] > FormatNEC::Turn)
	{
		if(PulseNEC[num] > FormatNEC::HighPeriod)
		{
			DecodeBuffer[0] |= 1 << numBits;
		}
		num++;
		numBits++;
	}

	DecodeBuffer[0] = ByteSwap(DecodeBuffer[0]);

	return 0;
}

uint32_t irDecode::DecodeAEHA(const uint16_t*Address)
{
	const uint16_t *PulseAEHA = Address;

	uint32_t num = 0;
	uint32_t numBits = 0;
	uint32_t idx = 0;

	memset(DecodeBuffer,0,sizeof(DecodeBuffer));

	while(PulseAEHA[num] > FormatAEHA::Turn)
	{
		if(PulseAEHA[num] > FormatAEHA::HighPeriod)
		{
			DecodeBuffer[idx] |= 1 << numBits;
		}
		num++;
		numBits++;
		idx = num/32;		//32ビットを超えた回数をカウント
		numBits %= 32;		//32ビットを超えたら0に戻す
	}

	for(uint32_t i = 0;i <= idx;i++)
	{
		DecodeBuffer[i] = ByteSwap(DecodeBuffer[i]);
	}

	num += 1;				//AEHAデコードではリーダーコードを判定していない為+1して返す
//	SetDecodeBit(num);		//AEHAフォーマットはビット数を記憶しておく

	return idx;
}



