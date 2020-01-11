#include "soil_moisture.h"
extern GPIO_InitTypeDef gpio_Init;
/*Init Mode OUTPUT cho RE & DE*/
void Soil_init(void){ 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
    
    gpio_Init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
	gpio_Init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    
	GPIO_Init(GPIOB,&gpio_Init);
}
void Soil_cmd(void){
    GPIO_WriteBit(GPIOB, RE,Bit_SET);
    GPIO_WriteBit(GPIOB, DE,Bit_SET);
    UART_SendChar(USART3,0x01);
    UART_SendChar(USART3,0x03);
    UART_SendChar(USART3,0x00);    
    UART_SendChar(USART3,0x01);    
    UART_SendChar(USART3,0x00);    
    UART_SendChar(USART3,0x01);    
    UART_SendChar(USART3,0xD5);    
    UART_SendChar(USART3,0xCA);
    delay_ms(11);
    GPIO_WriteBit(GPIOB,RE,Bit_RESET);
    GPIO_WriteBit(GPIOB,DE,Bit_RESET);
}
