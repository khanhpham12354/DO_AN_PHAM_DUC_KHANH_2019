#ifndef __SOIL_MOISTURE_H__
#define __SOIL_MOISTURE_H__

#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_usart.h"
#include "misc.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_syscfg.h"
#include "delay.h"
#include "usart.h"

#define         RE              GPIO_Pin_13 //PB13
#define         DE              GPIO_Pin_14 //PB14

void Soil_init(void);
void Soil_cmd(void);


#endif

