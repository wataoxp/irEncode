/*
 * model.cpp
 *
 *  Created on: Jun 12, 2026
 *      Author: wataoxp
 */

#include "clockmodel.h"
#include "watchclock.h"

using namespace RealClockSpace;
using namespace CKModel;
using namespace StateSymbol;

ClockModel::ClockModel(uint8_t lux) : Brighrness(lux),nAlarm{},nTime{}
{
	TargetPointers[static_cast<uint32_t>(TargetSymbol::TimeHours)] = &nTime.Hours;
	TargetPointers[static_cast<uint32_t>(TargetSymbol::TimeMinutes)] = &nTime.Minutes;

	TargetPointers[static_cast<uint32_t>(TargetSymbol::AlmaHours)] = &nAlarm.ALMA.Hours;
	TargetPointers[static_cast<uint32_t>(TargetSymbol::AlmaMinutes)] = &nAlarm.ALMA.Minutes;

	TargetPointers[static_cast<uint32_t>(TargetSymbol::AlmbHours)] = &nAlarm.ALMB.Hours;
	TargetPointers[static_cast<uint32_t>(TargetSymbol::AlmbMinutes)] = &nAlarm.ALMB.Minutes;
}

uint8_t ClockModel::SyncValue(TargetSymbol target,uint8_t max)
{
	using namespace WatchClock;

	uint32_t idx = static_cast<uint32_t>(target);
	uint8_t tmp = *(TargetPointers[idx]);	// 編集前の値を読み取る

	uint32_t dir = LL_TIM_GetDirection(RotaryEncoder::Timer);
	LL_TIM_SetCounter(RotaryEncoder::Timer, 0);		// エンコーダー読み取り時(しきい値判定)にクリアするのは早すぎて読み取れなくなる

	if(!dir)
	{
		tmp = (tmp < max)? tmp+1:0;
	}
	else
	{
		tmp = (tmp > 0)? tmp-1:max;
	}

	*(TargetPointers[idx]) = tmp;		// 新しいデータを書き込む

	return tmp;
}

// 除算＆剰余算の代用
void ClockModel::SplitDigits(uint8_t val,uint8_t& ten,uint8_t& unit)
{
	if 		(val >= 50) { ten = 5; unit = val - 50; }
	else if (val >= 40) { ten = 4; unit = val - 40; }
	else if (val >= 30) { ten = 3; unit = val - 30; }
	else if (val >= 20) { ten = 2; unit = val - 20; }
	else if (val >= 10) { ten = 1; unit = val - 10; }
	else 				{ ten = 0; unit = val; }
}

