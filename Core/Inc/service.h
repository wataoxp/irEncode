/*
 * service.h
 *
 *  Created on: Mar 7, 2026
 *      Author: wataoxp
 */

#ifndef INC_SERVICE_H_
#define INC_SERVICE_H_

#include "mylib.h"
#include "TM1652.h"
#include "so1602.h"
#include "encoder.h"

void UpdateSeg(RealClock& rtc,TM1652& seg,RealClockSpace::TimeRegBitField *tr);
void UpdateDisplay(SO1602& lcd,RealClockSpace::TimeRegBitField *tr);
void SetUserTime(TM1652& seg,Encoder& encode,RealClock& rtc,RealClockSpace::Options wut,uint32_t timing);
uint32_t SetTimeLoop(TM1652& seg,Encoder& encode,uint32_t min,uint32_t max,uint32_t FixedHour,bool isHour);


#endif /* INC_SERVICE_H_ */
