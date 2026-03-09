/*
 * so1602.c
 *
 *  Created on: Mar 1, 2025
 *      Author: wataoxp
 */
#include "so1602.h"
#include "delay.h"

SO1602::SO1602(I2C& i2c) :wire(i2c)
{
	;
}

template <typename wait>
uint32_t SO1602::Init(CoreClock Clock)
{
	using namespace SO1602Command;
	uint8_t config[] = {ClearDisplay,RetuenHome,DisplayCMD,ClearDisplay};
	uint8_t contrast[] = {0x2a,0x79,0x81,0xff,0x78,0x28};

	wait::mDelay(100);

	if(!wire.IsActiveDevice<wait>(Address))
	{
		while(1);
	}

	for(uint8_t i = 0;i < sizeof(config);i++)
	{
		wire.Write(Address, Sel_CMD, config[i]);
		wait::mDelay(20);
	}
	for(uint8_t j = 0;j < sizeof(contrast);j++)
	{
		wire.Write(Address, Sel_CMD, contrast[j]);
	}

	return 0;
}
template uint32_t SO1602::Init<DelayMode::Standard>(CoreClock Clock);
template uint32_t SO1602::Init<DelayMode::RtosMode>(CoreClock Clock);

void SO1602::PointClear(uint8_t y)
{
	char Clear[16];

	y &= 1;		// y座標のマスク

	for(uint8_t i = 0;i < sizeof(Clear);i++)
	{
		Clear[i] = 0xA0;		//半角スペース
	}
	SetCusor(0, y);
	StringLCD(Clear, sizeof(Clear));
	SetCusor(0, y);
}
