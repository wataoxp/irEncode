/*
 * state.cpp
 *
 *  Created on: Apr 24, 2026
 *      Author: wataoxp
 */

#include "state.h"
#include "watchclock.h"
#include "clockmodel.h"

using namespace WatchClock;

/* Idle */
StateIdle& StateIdle::Instance()
{
	static StateIdle instance;
	return instance;
}

ISystemState* StateIdle::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace RealClockSpace;
	using namespace ExtiPin;

	ISystemState* NextState = this;

	switch(cmd)
	{
	case SetTime:
		ckv.PrevTimeRead();
		NextState = &StateTimeHours::Instance();
		break;
	case SetAlma:
		ckv.PrevAlarmTimeView(AlarmType::ALMA);
		NextState = &StateAlmaHours::Instance();
		break;
	case SetAlmb:
		ckv.PrevAlarmTimeView(AlarmType::ALMB);
		NextState = &StateAlmbHours::Instance();
		break;
	default:
		break;
	}

	return NextState;
}

/* Hours */
StateTimeHours& StateTimeHours::Instance()
{
	static StateTimeHours instance;
	return instance;
}

ISystemState* StateTimeHours::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::TimeHours);

	return this;
}

ISystemState* StateTimeHours::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmHours(StateSymbol::TargetSymbol::TimeHours);
		return &StateTimeMinutes::Instance();
	}
	if(cmd == SetTime)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}

/* Minutes */
StateTimeMinutes& StateTimeMinutes::Instance()
{
	static StateTimeMinutes instance;
	return instance;
}

ISystemState* StateTimeMinutes::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::TimeMinutes);
	return this;
}

ISystemState* StateTimeMinutes::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmTime();
		return &StateIdle::Instance();
	}
	if(cmd == SetTime)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}

/* ALMA Hours */
StateAlmaHours& StateAlmaHours::Instance()
{
	static StateAlmaHours instance;
	return instance;
}

ISystemState* StateAlmaHours::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::AlmaHours);

	return this;
}

ISystemState* StateAlmaHours::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmHours(StateSymbol::TargetSymbol::AlmaHours);
		return &StateAlmaMinutes::Instance();
	}
	if(cmd == SetAlma)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}

/* ALMA Minutes */
StateAlmaMinutes& StateAlmaMinutes::Instance()
{
	static StateAlmaMinutes instance;
	return instance;
}

ISystemState* StateAlmaMinutes::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::AlmaMinutes);

	return this;
}

ISystemState* StateAlmaMinutes::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace RealClockSpace;
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmAlarm(AlarmType::ALMA);
		return &StateIdle::Instance();
	}
	if(cmd == SetAlma)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}

/* ALMB Hours */
StateAlmbHours& StateAlmbHours::Instance()
{
	static StateAlmbHours instance;
	return instance;
}

ISystemState* StateAlmbHours::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::AlmbHours);

	return this;
}

ISystemState* StateAlmbHours::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmHours(StateSymbol::TargetSymbol::AlmbHours);
		return &StateAlmbMinutes::Instance();
	}
	if(cmd == SetAlmb)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}

/* ALMB Minutes */
StateAlmbMinutes& StateAlmbMinutes::Instance()
{
	static StateAlmbMinutes instance;
	return instance;
}

ISystemState* StateAlmbMinutes::OnValueChange(ClockView& ckv)
{
	ckv.RotateValue(StateSymbol::TargetSymbol::AlmbMinutes);

	return this;
}

ISystemState* StateAlmbMinutes::OnCommand(uint32_t cmd,ClockView& ckv)
{
	using namespace RealClockSpace;
	using namespace ExtiPin;

	if(cmd == EncoderPush)
	{
		ckv.ConfirmAlarm(AlarmType::ALMB);
		return &StateIdle::Instance();
	}
	if(cmd == SetAlmb)
	{
		ckv.LoadTime();
		return &StateIdle::Instance();
	}
	return this;
}
