#ifndef __LORA_H__
#define __LORA_H__

#include "stm32l1xx.h"
#include "usart.h"
#include "delay.h"
#include "string.h"


void lora_enterTestMode(void);
void lora_enterLowpower(void);
void lora_enterWakeup(void);

#endif
