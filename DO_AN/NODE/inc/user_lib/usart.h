#ifndef __USART_H__
#define __USART_H__

#include "stm32l1xx.h"
#include "stm32l1xx_usart.h"
#include "stdio.h"

void UART_SendChar(USART_TypeDef *USARTx, char data);
void UART_PutStr(USART_TypeDef *USARTx, char *Str);
uint8_t USART_GetChar(USART_TypeDef* USARTx);



#endif
