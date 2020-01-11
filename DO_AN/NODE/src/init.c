#include "init.h"
/*-------------------------------------------*/
GPIO_InitTypeDef gpio_Init;
USART_InitTypeDef usart_Init;
/*-----------------Init----------------------*/
/*
    Iint config for ENABLE pins
    ADC: PA7
    SHT75: PA4
    LUX SENSOR: PB1
    SOIL MOISTURE: PB12
    RH_RST: PB3
    RH_PA0: PA15
*/
void InitPinEN_config(void){
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB,ENABLE);
    
    gpio_Init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_40MHz;
    
	gpio_Init.GPIO_Pin = GPIO_Pin_4| GPIO_Pin_7|GPIO_Pin_15|GPIO_Pin_1;
    GPIO_Init(GPIOA,&gpio_Init);
    
    gpio_Init.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_12| GPIO_Pin_3|GPIO_Pin_8;
    GPIO_Init(GPIOB,&gpio_Init);
    
    GPIO_SetBits(GPIOB,GPIO_Pin_3);
    GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}
/*
function: khoi tao cho USART1
    pin TX: PA9
    pin RX: PA10
*/
void Uart1_config(void){
	/*Reset USART1*/
	USART_DeInit(USART1);
	/*Enable clocl for usart1*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	/*config pin TX for and RX for USART1 is mode AF*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	gpio_Init.GPIO_Mode = GPIO_Mode_AF;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
	gpio_Init.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	
	GPIO_Init(GPIOA,&gpio_Init);
	/*Connect USART1 pins to AF7 (TX)*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	/*Connect USART1 pins to AF7 (RX) */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/*config params of USART1*/
	usart_Init.USART_BaudRate = 9600;
	usart_Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_Init.USART_WordLength = USART_WordLength_8b;
	usart_Init.USART_Parity = USART_Parity_No;
	usart_Init.USART_StopBits = USART_StopBits_1;
	
	USART_Init(USART1,&usart_Init);
	USART_Cmd(USART1, ENABLE);
}
/*
function: khoi tao cho USART2
    pin TX: PA2
    pin RX: PA3
*/
void Uart2_config(void){
	/*Reset USART2*/
	USART_DeInit(USART2);
	/*Enable clocl for usart2*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	/*config pin TX for and RX for USART12 is mode AF*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	gpio_Init.GPIO_Mode = GPIO_Mode_AF;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
	gpio_Init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	
	GPIO_Init(GPIOA,&gpio_Init);
	/*Connect USART1 pins to AF7 (TX)*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	/*Connect USART1 pins to AF7 (RX) */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/*config params of USART1*/
	usart_Init.USART_BaudRate = 9600;
	usart_Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_Init.USART_WordLength = USART_WordLength_8b;
	usart_Init.USART_Parity = USART_Parity_No;
	usart_Init.USART_StopBits = USART_StopBits_1;
	
	USART_Init(USART2,&usart_Init);
	USART_Cmd(USART2, ENABLE);
	/*Enable interrupt for USART2*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART2_IRQn);
}

/*
function: khoi tao cho USART3
    pin TX: PB11
    pin RX: PB10
*/
void Uart3_config(void){
	/*Reset USART1*/
	USART_DeInit(USART3);
	/*Enable clocl for usart1*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	/*config pin TX for and RX for USART3 is mode AF*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	gpio_Init.GPIO_Mode = GPIO_Mode_AF;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
	gpio_Init.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	
	GPIO_Init(GPIOB,&gpio_Init);
	/*Connect USART3 pins to AF7 (TX)*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	/*Connect USART3 pins to AF7 (RX) */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	/*config params of USART3*/
	usart_Init.USART_BaudRate = 9600;
	usart_Init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart_Init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usart_Init.USART_WordLength = USART_WordLength_8b;
	usart_Init.USART_Parity = USART_Parity_No;
	usart_Init.USART_StopBits = USART_StopBits_1;
	
	USART_Init(USART3,&usart_Init);
	USART_Cmd(USART3, ENABLE);
    
    /*Enable interrupt for USART3*/
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART3_IRQn);
}
/*
    fucntion: khoi tao nguon cho module Lora
    RH_EN: PA1
    RH_RST: PB3
    RH_PA0: PA15
*/
void Lora_PowerOn(void){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	gpio_Init.GPIO_Mode = GPIO_Mode_OUT;
	gpio_Init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_Init.GPIO_OType = GPIO_OType_PP;
	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
    
	gpio_Init.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB,&gpio_Init);
	
	gpio_Init.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_15;
	GPIO_Init(GPIOA,&gpio_Init);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_3);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_15);	
}
/*
enale power of sht75
    1 : enable
    0 : disable
PA4: SHT75_EN
*/
void SHT75_Enable(uint8_t en){
    en == 1 ? GPIO_ResetBits(GPIOA,GPIO_Pin_4): GPIO_SetBits(GPIOA,GPIO_Pin_4);
}
/*
enale power of ADC
    1 : enable
    0 : disable
PA7: ADC_EN
*/
void ADC_Enable(uint8_t en){
    en == 1 ? GPIO_ResetBits(GPIOA,GPIO_Pin_7): GPIO_SetBits(GPIOA,GPIO_Pin_7);

}
/*
enale power of LUX
    1 : enable
    0 : disable
PB1: LUX_EN
*/
void LUX_Enable(uint8_t en){
    en == 1 ? GPIO_ResetBits(GPIOB,GPIO_Pin_1): GPIO_SetBits(GPIOB,GPIO_Pin_1);
}
/*
enale power of SOIL
    1 : enable
    0 : disable
PB12: SOIL_EN
*/
void SOIL_Enable(uint8_t en){
    en == 1 ? GPIO_ResetBits(GPIOB,GPIO_Pin_12): GPIO_SetBits(GPIOB,GPIO_Pin_12);
}
/*
enale power of LORA
    1 : enable
    0 : disable
PB1: LORA_EN
*/
void LORA_Enable(uint8_t en){
    en == 1 ? GPIO_ResetBits(GPIOA,GPIO_Pin_1): GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
//void BTN_config(void){
//	/*enable clock for port A*/
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
//	/*config pin PA4 is mode output LED RGB*/
//	gpio_Init.GPIO_Mode = GPIO_Mode_IN;
//	gpio_Init.GPIO_PuPd = GPIO_PuPd_UP;
//	gpio_Init.GPIO_OType = GPIO_OType_PP;
//	gpio_Init.GPIO_Speed = GPIO_Speed_400KHz;
//	gpio_Init.GPIO_Pin = GPIO_Pin_4;
//    
//	GPIO_Init(GPIOA,&gpio_Init);
//}

//void EXTI0_config(void){
//    EXTI_InitTypeDef ext;
//    NVIC_InitTypeDef nvic;
//    
//    EXTI_DeInit();
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource4);
//    
//    ext.EXTI_Line = EXTI_Line4;
//    ext.EXTI_Mode = EXTI_Mode_Interrupt;
//    ext.EXTI_Trigger = EXTI_Trigger_Falling;
//    ext.EXTI_LineCmd = ENABLE;
//    
//    EXTI_Init(&ext);
//    
//    nvic.NVIC_IRQChannel = EXTI4_IRQn;
//    nvic.NVIC_IRQChannelSubPriority = 0x00;
//    nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
//    nvic.NVIC_IRQChannelCmd = ENABLE;

//    NVIC_Init(&nvic);

//}
