/*
 * service.h
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */

#ifndef INC_CLOCKVIEW_H_
#define INC_CLOCKVIEW_H_

#include "config.h"
#include "clockmodel.h"
#include "watchclock.h"

class ClockView{
private:
	RealClock sRTC;
	TM1652 sSeg;
	RealClockSpace::TimeRegBitField TimeReg;
	ClockModel ckm;

	void UpdateSeg(uint8_t value,uint8_t pos,uint8_t length,bool colon);
public:
	ClockView(RealClock& rtc,TM1652& seg,uint8_t lux);

	void PrevTimeRead();
	void PrevAlarmTimeView(RealClockSpace::AlarmType sel);

	void RotateValue(StateSymbol::TargetSymbol target);
	void ConfirmHours(StateSymbol::TargetSymbol target);
	void ConfirmTime();
	void ConfirmAlarm(RealClockSpace::AlarmType sel);

	void LoadTime();

	void UpdateTime();

	//	ServiceUnits::SystemState SetSegBrightness(const uint32_t Command,const bool diff);
};

inline void ClockView::LoadTime()
{
	while(LL_RTC_IsActiveFlag_RS(RTC) != 1);
	sRTC.GetTimeRegister(&TimeReg);
	sSeg.WriteDig(TimeReg.HourTens, TimeReg.HourUnits, TimeReg.MinuteTens, TimeReg.MinuteUnits);
}


#endif /* INC_CLOCKVIEW_H_ */
