#ifndef __PWR_H__
#define __PWR_H__
#include "stm32l1xx.h"
#include "stm32l1xx_pwr.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_flash.h"
#include "stm32l1xx_adc.h"

void PWR_Init(void);
void PWR_Lowpower(void);
void PWR_GPIO(void);
#endif

