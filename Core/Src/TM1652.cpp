/*
 * TM1652.cpp
 *
 *  Created on: Feb 6, 2026
 *      Author: wataoxp
 */

#include "TM1652.h"
#include <string.h>

using namespace Seg7;

TM1652::TM1652(UART& serial,DelayPoicy& pDelay) :uart(serial),delay(pDelay),CurrentBrightness(0)
{
	// ドットを含むと8セグ
	SelectSeg = Seg8Dig5;
}

/*** Private ***/

/* 1データ送信後は3ms以上のアイドル状態(UARTではHigh)を作る */
inline void TM1652::SendData(uint8_t *buf,uint8_t size)
{
	uart.TransmitData(buf, size);
	delay.mDelay(5);
}

// LSBtoMSB
uint8_t TM1652::ReverseByte(uint8_t b)
{
	b = (b & 0b11110000) >> 4 | (b & 0b00001111) << 4;
	b = (b & 0b11001100) >> 2 | (b & 0b00110011) << 2;
	b = (b & 0b10101010) >> 1 | (b & 0b01010101) << 1;

	return b;
}

inline uint8_t TM1652::SetAmpereValue(uint8_t ampere)
{
	// ampereの有効値は0～7なので‐1する
	return (ReverseByte(MinDuty) | (ReverseByte(ampere-1) >> AmperePos));
}

inline uint8_t TM1652::SetDutyValue(uint8_t value)
{
	uint8_t duty = (value - MaxAmpere);	// Duty設定値を抽出
	return (ReverseByte(duty) | (ReverseByte(MaxAmpere) >> AmperePos));
}

/*** Public ***/

void TM1652::Clear(void)
{
	uint8_t clear[5] = {0};
	clear[0] = CommandBase;

	SendData(clear, sizeof(clear));
	delay.mDelay(15);
}

uint8_t TM1652::GetBrightness(void)
{
	return CurrentBrightness;
}

// 駆動電流とデューティー比を設定
uint32_t TM1652::SetBrightness(uint8_t brightness)
{
	uint8_t data[2];

	data[0] = CommandBase | DispCommand;

	if(brightness == 0)
	{
		data[1] = 0;	// 消灯
	}
	else if(brightness <= AmpereStepLimit)
	{
		data[1] = SetAmpereValue(brightness);
	}
	else if(brightness <= MaxBright)
	{
		data[1] = SetDutyValue(brightness);
	}
	else
	{
		return OverFlow;
	}

	data[1] |= this->SelectSeg;

	SendData(data, sizeof(data));

	CurrentBrightness = brightness;

	return Success;
}

void TM1652::WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4,bool colon)
{
	uint8_t data[4] = {d1,d2,d3,d4};

	WriteDigPos(data, 1, 4,colon);
}

uint32_t TM1652::WriteDigPos(uint8_t* data,uint8_t pos,uint8_t length,bool colon)
{
	uint8_t buffer[5];

	if(!pos || length > MaxDigNum) return Failed;

	buffer[0] = CommandBase | ReverseByte(pos-1);

	for (uint8_t i = 0; i < length; i++)
	{
		buffer[i + 1] = DigNum[(data[i])];
	}

	if(colon && pos <= ColonPos)	// GR2への書き込み時のみコロンを操作できる
	{
		buffer[2] |= 0x80;
	}

	SendData(buffer, length+1);

	return Success;
}

