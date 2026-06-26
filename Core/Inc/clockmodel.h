/*
 * model.h
 *
 *  Created on: Jun 12, 2026
 *      Author: wataoxp
 */

#ifndef INC_CLOCKMODEL_H_
#define INC_CLOCKMODEL_H_

#include "config.h"

namespace CKModel{
	typedef struct{
		uint8_t Hours;
		uint8_t Minutes;
	}TimeFiled;
}

namespace StateSymbol{
	enum class TargetSymbol{
		TimeHours = 0,
		TimeMinutes,
		AlmaHours,
		AlmaMinutes,
		AlmbHours,
		AlmbMinutes,
		MaxTargetsNumber,		// 配列の番兵(末尾)
	};
}

class ClockModel{
private:
	uint8_t Brighrness;
	RealClockSpace::AlarmParameters nAlarm;
	CKModel::TimeFiled nTime;
	uint8_t* TargetPointers[static_cast<uint32_t>(StateSymbol::TargetSymbol::MaxTargetsNumber)];
public:
	ClockModel(uint8_t lux);

	uint8_t SyncValue(StateSymbol::TargetSymbol target,uint8_t max);
	void SplitDigits(uint8_t val,uint8_t& ten,uint8_t& unit);

	uint8_t GetStatus(StateSymbol::TargetSymbol target);

	// PrevAlarmTimeView等で使用されているので、これをどう取得していくか

	inline uint8_t& GetTimeHours() {return nTime.Hours;}
	inline uint8_t& GetTimeMinutes() {return nTime.Minutes;}

	inline uint8_t& GetAlmaHours() {return nAlarm.ALMA.Hours;}
	inline uint8_t& GetAlmaMinutes() {return nAlarm.ALMA.Minutes;}

	inline uint8_t& GetAlmbHours() {return nAlarm.ALMB.Hours;}
	inline uint8_t& GetAlmbMinutes() {return nAlarm.ALMB.Minutes;}

	inline CKModel::TimeFiled& GetTimeStatus() {return nTime;}
	inline RealClockSpace::AlarmParameters& GetAlarmStatus() {return nAlarm;}
};

inline uint8_t ClockModel::GetStatus(StateSymbol::TargetSymbol target)
{
	return *(TargetPointers[static_cast<uint32_t>(target)]);
}


#endif /* INC_CLOCKMODEL_H_ */
