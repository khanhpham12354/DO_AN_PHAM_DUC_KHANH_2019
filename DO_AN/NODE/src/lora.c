#include "lora.h"
void lora_enterTestMode(void){
    
    UART_PutStr(USART2, "AT\r\n");
    delay_ms(100);
    
    UART_PutStr(USART2, "AT+MODE=TEST\r\n");
    delay_ms(100);
    // Set RF Configurations
    UART_PutStr(USART2,"AT+TEST=RFCFG,433\r\n");
    delay_ms(100);
}
void lora_enterLowpower(void){
    UART_PutStr(USART2, "AT+LOWPOWER\r\n");
    delay_ms(100);
}
void lora_enterWakeup(void){
    printf("A"); // Send any character to wake-up the modem
    delay_ms(10); // Wait modem ready
}
