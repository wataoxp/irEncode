/*
 * watchclock.h
 *
 *  Created on: Apr 24, 2026
 *      Author: wataoxp
 */

#ifndef INC_WATCHCLOCK_H_
#define INC_WATCHCLOCK_H_

#include "mylib.h"

namespace WatchClock {
	// C++では構造体でもコンストラクタが呼ばれるのでdeleteでインスタンス化を禁止する
	namespace ExtiPin{
		GPIO_TypeDef* const GPIOx = GPIOB;

		enum PinPos{
			PushPos			= Pin0,
			TimePos			= Pin1,
			AlmaPos			= Pin2,
			AlmbPos			= Pin3,
			BrightnessPos	= Pin4,
		};

		enum Value{
			EncoderPush		= 1 << PushPos,
			SetTime			= 1 << TimePos,
			SetAlma			= 1 << AlmaPos,
			SetAlmb			= 1 << AlmbPos,
			SetBrightness	= 1 << BrightnessPos,
		};

		enum ExtiLine{
			LinePush   		= LL_EXTI_LINE_0,
			LineTime       	= LL_EXTI_LINE_1,
			LineAlma     	= LL_EXTI_LINE_2,
			LineAlmb     	= LL_EXTI_LINE_3,
			LineBrightness 	= LL_EXTI_LINE_4,
		};

		constexpr static IRQn_Type IRQn1 = EXTI0_1_IRQn;
		constexpr static IRQn_Type IRQn2 = EXTI2_3_IRQn;
		constexpr static IRQn_Type IRQn3 = EXTI4_15_IRQn;

		// 全EXTIピンのビットマスク
		constexpr uint32_t GetAllExtiMask() {
			return  (EncoderPush) 	|
					(SetTime)	 	|
					(SetAlma)	 	|
					(SetAlmb)	 	|
					(SetBrightness);
		}

		constexpr uint32_t GetAllExtiLine() {
			return 	(LinePush) |
					(LineTime) |
					(LineAlma) |
					(LineAlmb) |
					(LineBrightness);
		}
	};

	namespace SegSerial{
		USART_TypeDef* const UARTx	= USART1;
		GPIO_TypeDef* const GPIOx 	= GPIOB;

		enum PinPos{
			TxPos	= Pin6,
			RxPos	= Pin7,
		};

		enum Alternate{
			TxAf	= LL_GPIO_AF_0,
			RxAf	= LL_GPIO_AF_0,
		};
	}


	namespace RotaryEncoder{
		TIM_TypeDef* const Timer  = TIM3;
		GPIO_TypeDef* const GPIOx = GPIOA;

		enum PinPos{
			PhaseAPos	= Pin6,
			PhaseBPos	= Pin7,
		};
		enum Alternate{
			AfPhaseA	= LL_GPIO_AF_1,
			AfPhaseB	= LL_GPIO_AF_1,
		};
	}

	namespace Indicator{
		GPIO_TypeDef* const GPIOx = GPIOC;

		enum PinPos{
			IndicatorPos = Pin6,
		};

		enum Value{
			Indicator 	 = 1 << IndicatorPos,
		};
	}

    namespace Parameter{
        // 明るさ設定
        constexpr uint32_t DefaultBrightness() { return 2U; }
        constexpr uint32_t MaxBrightness()     { return 7U; }
        constexpr uint32_t MinBrightness()     { return 0U; }

        // RTC関連
        constexpr uint32_t WakeUpCount() { return 59; }
        constexpr uint32_t WutFlagVal  = UINT8_MAX << 24;
        constexpr uint32_t AlmaFlagVal = UINT8_MAX << 16;
        constexpr uint32_t AlmbFlagVal = UINT8_MAX << 8;

        constexpr uint32_t InputLimitMillTime() { return 5000; }
    }

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
}


#endif /* INC_WATCHCLOCK_H_ */
