/*
 * service.cpp
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */

#include <clockview.h>
#include <cassert>

struct SegComponent{
	uint8_t MaxVal;
	uint8_t SegPos;
	bool isColon;
};

const static SegComponent unit[] = {
	{ 23,   1,   false },
	{ 59,   3,   true },	// Time
	{ 23,   1,   false },
	{ 59,   3,   true },	// ALMA
	{ 23,   1,   false },
	{ 59,   3,   true },	// ALMB
};

using namespace CKModel;

ClockView::ClockView(RealClock& rtc,TM1652& seg,uint8_t lux) : sRTC(rtc),sSeg(seg),TimeReg{},ckm(lux)
{
	;
}

/*** Private ***/

// 2ケタずつ更新
void ClockView::UpdateSeg(uint8_t value,uint8_t pos,uint8_t length,bool colon)
{
	assert(length <= 2);

	uint8_t Time[2];
	ckm.SplitDigits(value, Time[0], Time[1]);
	sSeg.WriteDigPos(Time, pos, length,colon);
}

/*** Public ***/

// 現在の時刻を読み込む
void ClockView::PrevTimeRead(void)
{
	TimeFiled& nTime = ckm.GetTimeStatus();

	nTime.Hours = (TimeReg.HourTens*10) + TimeReg.HourUnits;
	nTime.Minutes = (TimeReg.MinuteTens*10) + TimeReg.MinuteUnits;
}

// 前回の設定値を読み込んで表示する
void ClockView::PrevAlarmTimeView(RealClockSpace::AlarmType sel)
{
	using namespace RealClockSpace;

	uint8_t& hours = (sel == AlarmType::ALMA)? ckm.GetAlmaHours():ckm.GetAlmbHours();
	uint8_t& minutes = (sel == AlarmType::ALMA)? ckm.GetAlmaMinutes():ckm.GetAlmbMinutes();

	uint8_t time[4];
	ckm.SplitDigits(hours, time[0], time[1]);
	ckm.SplitDigits(minutes, time[2], time[3]);
	sSeg.WriteDigPos(time, 1, 4, false);
}

void ClockView::RotateValue(StateSymbol::TargetSymbol target)
{
	uint32_t idx = static_cast<uint32_t>(target);
	const SegComponent& cmp = unit[idx];	// idx番目を読み取るだけ(参照とすることでコピーを回避)
	uint8_t val = ckm.SyncValue(target, cmp.MaxVal);

	UpdateSeg(val,cmp.SegPos, 2, cmp.isColon);
}

void ClockView::ConfirmHours(StateSymbol::TargetSymbol target)
{
	UpdateSeg(ckm.GetStatus(target), 1, 2, true);
}

void ClockView::ConfirmTime()
{
	using namespace WatchClock;

	sRTC.SetTime(LL_RTC_TIME_FORMAT_AM_OR_24, ckm.GetTimeHours(), ckm.GetTimeMinutes(), 0);
	sRTC.SetWakeUpTimer(Parameter::WakeUpCount());	// ウェイクアップタイマをリセット
	LoadTime();
}

void ClockView::ConfirmAlarm(RealClockSpace::AlarmType sel)
{
	using namespace RealClockSpace;

	AlarmParameters& nAlarm = ckm.GetAlarmStatus();

	if(sel == AlarmType::ALMA)
	{
		nAlarm.ALMA.Seconds = 0;
		nAlarm.ALMA.Mask = GetAlmaMaskMinutes();
	}
	else
	{
		nAlarm.ALMB.Seconds = 0;
		nAlarm.ALMB.Mask = GetAlmbMaskMinutes();
	}
	nAlarm.SelectAlarm = sel;

	// 参照でもポインタとして渡せる
	sRTC.SetAlarm(&nAlarm);
	LoadTime();
}

// 1分毎に更新
void ClockView::UpdateTime()
{
	while(LL_RTC_IsActiveFlag_RS(RTC) != 1);
	sRTC.GetTimeRegister(&TimeReg);

	if((TimeReg.SecondTens + TimeReg.SecondUnits) != 0)
	{
		// 0秒でないなら再読み込み
		__NOP();
		LL_RTC_ClearFlag_RS(RTC);
		while(LL_RTC_IsActiveFlag_RS(RTC) != 1);
		sRTC.GetTimeRegister(&TimeReg);
	}
	sSeg.WriteDig(TimeReg.HourTens, TimeReg.HourUnits, TimeReg.MinuteTens, TimeReg.MinuteUnits);
}

//SystemState ClockView::SetSegBrightness(const uint32_t Command,const bool diff)
//{
//	using namespace WatchClock::ExtiPin;
//
//	SystemState state = StateBrightness;
//
//	if(diff)
//	{
//		sBrighrness = SetUserValue(0, Seg7::MaxBright, sBrighrness);
//		sSeg.SetBrightness(sBrighrness);
//		UpdateSeg(sBrighrness, 3, 2, false);
//
//	}
//	if(Command == (1 << EncoderPush) || Command == (1 << SetBrightness))
//	{
//		UpdateTime();
//		state = StateIdle;
//	}
//
//	return state;
//}
