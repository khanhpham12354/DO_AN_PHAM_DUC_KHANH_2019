#include "usart.h"

struct __FILE {
    int dummy;
};
FILE __stdout;

void UART_SendChar(USART_TypeDef *USARTx, char data){
    USARTx->DR &= 0x00000000;
    USART_SendData(USARTx,data);
    //TxE = 1: Data is transferred to the shift register)
    //TxE = 0; Data is not transferred to the shift register
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}
void UART_PutStr(USART_TypeDef *USARTx, char *Str){
    while(*Str){
        UART_SendChar(USARTx, *Str);
        Str++;
    }
}
uint8_t USART_GetChar(USART_TypeDef* USARTx){
    uint8_t Data;
    while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
    Data = (uint8_t)USART_ReceiveData(USARTx);
    return Data;
}

int fputc(int ch, FILE *f){
	/* Send your custom byte */
	USART_SendData(USART2,ch);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){};
	/* If everything is OK, you have to return character written */
	return ch;
}



