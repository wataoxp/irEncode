/*
 * config.h
 *
 *  Created on: Jun 26, 2026
 *      Author: wataoxp
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_


namespace SendIR {
	namespace LfTimer{
		TIM_TypeDef* const Timer = TIM16;	// 基準周波数
		enum Parameter{
			Channel		= LL_TIM_CHANNEL_CH1,
			Period	   	= 1000,		// DMAで変更される
			Prescaler 	= 16,		// 1us分解能
		};

		GPIO_TypeDef* const GPIOx = GPIOB;
		enum PinConfig{
			PinPos 		= Pin8,
			Alternate	= LL_GPIO_AF_2,
		};
	};

	namespace HfTimer{
		TIM_TypeDef* const Timer = TIM17;	// 38kHzキャリア周波数

		enum Parameter{
			Channel 	= LL_TIM_CHANNEL_CH1,
			Period 		= 413,		// 16M/413 = 38.74kHz
			Duty 		= 3,		// デューティー比30%
			Prescaler 	= 1,		// リロード値で周波数は設定済み
		};

		GPIO_TypeDef* const GPIOx = GPIOB;
		enum PinConfig{
			PinPos 		= Pin9,
			Alternate	= LL_GPIO_AF_0,
		};
	};

	namespace MemoryAccess{
		DMA_TypeDef* const Handle = DMA1;
		enum Parameter{
			Channel 	= LL_DMA_CHANNEL_2,
		};
	}
}



#endif /* INC_CONFIG_H_ */
