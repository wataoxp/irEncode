/*
 * TM1652.h
 *
 *  Created on: Feb 6, 2026
 *      Author: wataoxp
 */

#ifndef INC_TM1652_H_
#define INC_TM1652_H_

#include "uart.h"
#include "delay.h"
#include <stdint.h>

namespace Seg7{

	// 0~9の点灯パターン
	constexpr uint8_t DigNum[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
	constexpr uint8_t MaxDigNum = 4;
	constexpr uint8_t ColonPos = 2;

	// すべてのコマンドは下位4ビットを0b1000とする
	typedef enum :uint8_t{
		CommandBase = 1 << 3,
		DispCommand = 1 << 4,	// 表示制御コマンド(display control command)
	}Commands;

	// 備考1
	typedef enum :uint8_t{
		DutyMask = 0xF0,
		AmpereMask = 0x07,
		ModeMask = 0x01,
		AmperePos = 4,
	}DispCommands;

	// 備考2
	typedef enum :uint8_t{
		MinAmpere = 0,
		MaxAmpere = 7,
		AmpereStepLimit = MaxAmpere+1,	// ampereは8ステップ
		MinDuty = 1,
		MaxDuty = 15,
		DutyStepLimit = MaxDuty,		// Dutyは15ステップ(0は消灯)

		MaxBright = MaxAmpere+MaxDuty,
	}DriveStatus;

	typedef enum :uint32_t{
		Success = 0,
		Failed,
		UnderFlow,
		OverFlow,
	}Seg7RetrunCode;

	typedef enum :uint8_t{
		Seg8Dig5 = 0,
		Seg7Dig6 = 1,
	}SegmentMode;
}

class TM1652{
private:
	UART& uart;
	DelayPoicy& delay;
	Seg7::SegmentMode SelectSeg;
	uint8_t CurrentBrightness;
	void SendData(uint8_t *buf,uint8_t size);
	uint8_t ReverseByte(uint8_t b);
	uint8_t SetAmpereValue(uint8_t ampere);
	uint8_t SetDutyValue(uint8_t value);
public:
	TM1652(UART& serial,DelayPoicy& pDelay);
	void Clear(void);
	uint8_t GetBrightness(void);
	void SetCusor(uint8_t cusor);
	uint32_t SetBrightness(uint8_t brightness);
	void WriteDig(uint8_t d1,uint8_t d2,uint8_t d3,uint8_t d4,bool colon = false);
	uint32_t WriteDigPos(uint8_t* data,uint8_t pos,uint8_t length,bool colon = false);
};

/*** 備考欄 ***
 * 備考1
 * xxxx0000->Duty比
 * 0000xxx0->セグメント駆動電流
 * 0000000x->セグ数の選択(SelectSeg)
 *
 * 備考2
 * CurrentAmpere(電流)は0~7。Dutyは1~15までの設定値を取る(データシートP6)
 * Brightness設定関数での有効値は0~22までの23個の数値。
 * 同関数の引数が0ならこれは消灯とみなす。
 */



#endif /* INC_TM1652_H_ */
