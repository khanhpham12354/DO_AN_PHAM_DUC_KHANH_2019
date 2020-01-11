#ifndef __STM32_RTC_H__
#define __STM32_RTC_H__

#include "stm32l1xx.h"
#include "stm32l1xx_rtc.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_pwr.h"
#include "misc.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_syscfg.h"

void RTC_Domain_access(void);
void RTC_setup(void);
void RTC_setupTime(uint8_t hour,uint8_t minute,uint8_t second);
void RTC_setupDate(uint8_t weekday,uint8_t date,uint8_t month,uint8_t year);
void RTC_Alarm_SetIT(void);
void RTC_Alarm_setTime(uint8_t m,uint8_t s);
#endif
