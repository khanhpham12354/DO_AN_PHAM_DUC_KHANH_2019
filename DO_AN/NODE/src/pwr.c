#include "pwr.h"

extern GPIO_InitTypeDef gpio_Init;

void PWR_Init(void){
    /* Enable Clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    while(PWR_GetFlagStatus(PWR_FLAG_VOS) == SET);
    /*Vcore = 1.8V*/
    PWR_VoltageScalingConfig(PWR_VoltageScaling_Range2);
   /* Disable prefetch buffer */
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
    /* Enable flash instruction cache*/
    FLASH->ACR |= (0x01<<3);
    FLASH->ACR |= (0x01<<4);
    /*Turn off ADC*/    
    ADC1->CR2 |= ~(0x01<<0);
    /*Turn off Temperature*/
    ADC->CCR &= (uint32_t)(~ADC_CCR_TSVREFE);
}
void PWR_Lowpower(void){
    /* Prepare to enter stop mode */
    PWR->CR |= PWR_CR_CWUF; // clear the WUF flag after 2 clock cycles
    PWR->CR &= ~( PWR_CR_PDDS ); // Enter stop mode when the CPU enters deepsleep  
    PWR->CR |= (0x01<<9);// disable interrupt VRIF
    PWR_UltraLowPowerCmd(ENABLE);
    PWR->CR &= 0xFFFFFFEF;// disable interrupt VRIF
    PWR_FastWakeUpCmd(ENABLE);
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // low-power mode = stop mode
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}
void PWR_GPIO(void){
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD,ENABLE);
   
    gpio_Init.GPIO_Mode = GPIO_Mode_AIN;
    gpio_Init.GPIO_Pin = GPIO_Pin_All;
    
    GPIO_Init(GPIOB,&gpio_Init);
    GPIO_Init(GPIOC,&gpio_Init);
    GPIO_Init(GPIOD,&gpio_Init);
    
    gpio_Init.GPIO_Mode = GPIO_Mode_AIN;
    gpio_Init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5| GPIO_Pin_6 
    | GPIO_Pin_7 |GPIO_Pin_8|GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_15;
    
    GPIO_Init(GPIOA,&gpio_Init);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD,DISABLE);
}

