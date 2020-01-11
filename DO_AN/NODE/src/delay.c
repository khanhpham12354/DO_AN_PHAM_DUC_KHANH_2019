/*
    File Name: delay.c
    Author: khanh pham
    Date: 21-07-2019
*/
#include "delay.h"

/*note: you must call fucntion SystemCoreClockUpdate() after this fucntion */
void SystemClock_Config(void){
    RCC_DeInit();
    RCC_HSICmd(ENABLE);
    /*HCLK = SYSCLK*/
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    /*PCLK2 = HCLK*/
    RCC_PCLK2Config(RCC_SYSCLK_Div1);
    /*PCLK1 = HCLK*/
    RCC_PCLK1Config(RCC_SYSCLK_Div1);
    /*use HSI as system source*/
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    /*wait still HSI is used as system clock source*/
    while(RCC_GetSYSCLKSource() != 0x04){};
}

void Systick_init(void){
    /*Reset systick*/
    SysTick->CTRL = 0x00000000;
    /*SystemCoreClock*/
    SysTick->CTRL |= (1<<2);
    SysTick->VAL = 0x000000;	
}

void Systick_start(uint32_t value){
    SysTick->LOAD = value;
    SysTick->VAL = 0x000000;
    SysTick->CTRL |= (1<<0);
}

void delay_us(uint32_t time){
    while(time--){
        Systick_start(0xF);
        while((SysTick->CTRL & (1<<16))==0);
    }
}

void delay_ms(uint32_t time){
    while(time--){
        Systick_start(0x3E7F);
        while((SysTick->CTRL & (1<<16))==0);
    }
}

