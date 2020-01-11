/*
    File Name: delay.h
    Author: khanh pham
    Date: 21-07-2019
*/
#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"

void SystemClock_Config(void);
void Systick_init(void);
void Systick_start(uint32_t value);
void delay_us(uint32_t time);
void delay_ms(uint32_t time);

#endif

