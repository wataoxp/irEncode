/*
 * so1602.h
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */

#ifndef INC_SO1602_H_
#define INC_SO1602_H_

#include "i2c.h"

namespace SO1602Command{
	//SA0 == Low
	constexpr uint8_t Address = 0x3C << 1;

	typedef enum : uint8_t{
		Sel_CMD = 0x00,
		Sel_DATA = 0x40,

		ClearDisplay = 0x01,
		RetuenHome = 0x02,

		DisplayOn = 0x04,
		CusorOn = 0x02,
		BlinkCusor = 0x01,
		DisplayCMD = 0x08 | DisplayOn,
		DDRAM_Access = 0x80,
	}RegAndCommand;

	typedef enum : uint8_t{
		/* SO1602 DDRAM */
		/****************
		 * Row1|0x00~0x0F
		 * Row2|0x20~0x2F
		 ****************/
		HomeCusor = 0x00,
		EnterCusor = 0x20,
	}CusorPosition;
}

typedef void (*mDelay)(uint32_t);

class SO1602{
private:
	I2C& wire;
public:
	SO1602(I2C& i2c);

	template <typename wait>
	uint32_t Init(CoreClock Clock);
	void PointClear(uint8_t y);

	void StringLCD(const char *str,uint8_t size);
	void ClearLCD(void);
	void SetCusor(uint8_t x,uint8_t y);
	void SendCMD(uint8_t cmd);
};

inline void SO1602::StringLCD(const char *str,uint8_t size)
{
	wire.MemWrite(SO1602Command::Address, SO1602Command::Sel_DATA, Wires::MemAddSize8, (uint8_t*)str, size);
}

inline void SO1602::ClearLCD(void)
{
	wire.Write(SO1602Command::Address, SO1602Command::Sel_CMD, SO1602Command::ClearDisplay);
}

inline void SO1602::SetCusor(uint8_t x,uint8_t y)
{
	wire.Write(SO1602Command::Address, SO1602Command::Sel_CMD, (SO1602Command::DDRAM_Access | (x + y * SO1602Command::EnterCusor)));
}

inline void SO1602::SendCMD(uint8_t cmd)
{
	wire.Write(SO1602Command::Address, SO1602Command::Sel_CMD, cmd);
}

#endif /* INC_SO1602_H_ */
