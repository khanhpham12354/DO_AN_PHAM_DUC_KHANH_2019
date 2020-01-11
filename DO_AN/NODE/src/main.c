/*
    File Name: main.c
    Author: khanh pham
    Date: 21-07-2019
*/

#include "stm32l1xx.h"
#include "delay.h"
#include "init.h"
#include "usart.h"
#include "lora.h"
#include <stdio.h>
#include <stdlib.h>
#include "soil_moisture.h"
#include "sht75.h"
#include "stm32_rtc.h"
#include "pwr.h"
#include "ph.h"
#include "kalman.h"


#define SIZE                256

#define Offset              0.0           // deviation compensate - Check to ensure the error does not exceed 0.3.
#define Offset_axit         0.00
#define Offset_bazo         0.00

#define ArrayLenth          50    // times of collection
// kalman
float _err_measure = 1, _err_estimate = 1, _q = 0.1;
double _current_estimate, _last_estimate, _kalman_gain;
int pHArray[ArrayLenth];   // Store the average value of the sensor feedback
void ADC_Configuration(void);
double avergearray(int* arr, int number);
uint16_t Get_Avarage(void);
double voltage, pHValue,getvalue,a;

PH_InitTypeDef PH_InitStructure;
/*
	USART1																										ADC1
	TX: PA9																										PA1
	RX:	PA10
	
	- range PH (0 to 14) - In 0 to 14pH range, the output voltage of the electrode is linear.
	- In order to ensure the accuracy of the pH probe, you need to use the standard solution to calibrate it regularly
	- Generally, the period is about half a year.
	- If you meaure the dirty aqueous solution, you need to increase the frequency of calibration.
			
Specification :

	+ Module Power: 5.00V
	+ Module Size: 43mmx32mm
	+ Measuring Range: 0-14PH
	+ Measuring Temperature: 0-60 C
	+ Accuracy: Ѡ0.1pH (25 C)
	+ Response Time: = 1min
	+ Industry pH Electrode with BNC Connector
	+ PH2.0 Interface ( 3 foot patch )
	+ Gain Adjustment Potentiometer
	+ Power Indicator LED	

electrode : dien cuc

*/

/*

	Note:If you want to measure the pH value of other solution,you must wash the pH electrode first!
	+ neu electrode lau khong duoc su dung phai ngam trong dd 3NKCL trong 8h
	+ electrode khong dc ngam lau trong dung dich axit clorua
	+ electrode khi su dung phai loai bo nhung thu bam tren electrode
	+ Moi truong co the lam ban electrode => sai so lon, vi vay can nhac yeu to lam sach
	
	PH
	|						y = mx + b (linear)
	| .
	|  .
	|   .
7	|    .
	|			.
4	|			 .
	|				.
2	|				 .
	|				  .
	|--------------------------------------------------------------> analog(x)
	0 																																V
	
	Xac dinh cong thuc do PH:
	
	point 4 <=>	a V	-- VD a = 0.615V
	point 7 <=> c V	-- VD b = 0.455V
	
	y = mx + b
			
								4-7				    3
	m(slope) = ---------- = ---------			= -18.75
							a   -  b		0.615-0.455
	b: intecept --> b = ((m*c)-7) = |((-18.75*0.455)-7)| = 15.53
	
	x : analogRead();
	
	=> PH = 5 -> y = -18.75*0.5 + 15.53
	
	note: put the electrode after one minutes => calibaration
	
	adjust axit --> test dung dich 4.00
	adjust bazo --> test dung dich 9.18
*/

/*define pin LEDs*/
#define         LED             GPIO_Pin_8  //PB8

char UID_TX[SIZE]= "G04";
char TX_buf[SIZE];

volatile uint8_t old_buf;
volatile uint8_t RxBuffer, array[8];
int i = 0;

uint8_t en = 1,j = 0,start = 0;

char value[10];

float temperature = 0, humidity=0, dewpoint=0;
uint16_t rawData;

char Buffer[7];
char Battery[7];
char Battery_1[7]="0";
    
int main(){
    
    SystemClock_Config();
	SystemCoreClockUpdate();
    Systick_init();
    
    PWR_Init();   
    PWR_GPIO();
    InitPinEN_config();
	Uart1_config();
	Uart2_config();
    delay_ms(1500);
    lora_enterTestMode();
    Soil_init();
    Uart3_config();
    LORA_Enable(1);
    SOIL_Enable(1);
    ADC_Enable(0);
    LUX_Enable(0);
    SHT75_Setup();
    SHT75_Enable(1);
    
    ADC_Configuration();
    delay_ms(1000);
	InitPH(&PH_InitStructure);
//    RTC_DeInit();
//    RTC_Domain_access();
//    RTC_setup();
//    RTC_setupTime(0,0,0);
//    RTC_Alarm_SetIT();
//    RTC_Alarm_setTime(1,0);
//    RTC_ClearITPendingBit(RTC_IT_ALRA);
    while(1){
        /*doc pin*/
        UART_PutStr(USART2,"AT+VDD\r\n");
        delay_ms(100);	
        if(en==1){
            memcpy(Battery,Buffer+5,4);
            strcpy(Battery_1,Battery);
       
            //UART_PutStr(USART1,Battery_1);
            memset(Battery,0,sizeof(Battery));
            memset(Buffer,0,sizeof(Buffer));
            
            USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
            en  = 0;
        }
        /*doc nhiet do do am*/
        SHT75_measTemp(&rawData);  
		temperature = SHT75_calcTemp(rawData);
		SHT75_measHumi(&rawData); 
		humidity    = SHT75_calcHumi(rawData, temperature);
		dewpoint    = SHT75_calcDewpoint(humidity, temperature);
        /*doc PH*/
        getvalue = avergearray(pHArray, 20);
		voltage = (avergearray(pHArray, 20)/1024.0)*3.3;// read the voltage - VREF+ : 3.3V
        int pHValue  = 0 ;
		pHValue =	3.5*(voltage) + Offset;
        /*doc do am dat*/
        Soil_cmd();
        delay_ms(1000);
        sprintf(value,"%X%X",array[3],array[4]);
        int soil = (int)strtol(value, NULL, 16);
        /*gui du lieu qua Lora*/
        sprintf(TX_buf,"%s_%d_%d_%d_%d_%d_%s\n", UID_TX,(int)(soil/100),(int)temperature,(int)humidity,(int)dewpoint,pHValue,Battery_1);
        if((int)(soil/100)>=0 && (int)(soil/100) <=100){
            lora_enterTestMode();
            printf("AT+TEST=TXLRSTR,%s\r\n",TX_buf);
        }
        else{
            UART_PutStr(USART1,TX_buf);
          //  soil = 0;
            //memset(value,0,sizeof(value));
            SOIL_Enable(0);
            delay_ms(100);
            SOIL_Enable(1);
            delay_ms(100);
            
        }
        
        memset(Battery_1,0,sizeof(Battery_1));
        /*Dua vao che do ngu*/
//        delay_ms(100);
//        printf("AT+RESET\r\n");
//        delay_ms(2000);
//        printf("AT+LOWPOWER\r\n");
//        delay_ms(100);
//        PWR_Lowpower();
//        
//        SystemClock_Config();
//        SystemCoreClockUpdate();
//        Systick_init();
//        
        
        delay_ms(4000);
        GPIO_ToggleBits(GPIOB,LED);
     //   printf("AT\r\n");
    }   
}
void RTC_Alarm_IRQHandler(void){
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET){
    /* Reconfig RTC times */
     RTC_setupTime(0,0,0);
    /* Clear RTC AlarmA Flags */
    RTC_ClearITPendingBit(RTC_IT_ALRA);  
    /* Clear the EXTIL line 17 */
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
}
void USART2_IRQHandler(){
	volatile uint8_t RxBuffer = (uint8_t)USART_GetChar(USART2);
	UART_SendChar(USART1,RxBuffer);
    if(old_buf ==0x2B && RxBuffer==0x56){
        start = 1;
	}
    if(start == 1){
        Buffer[j] = RxBuffer;
        j++;
        if(Buffer[j-1]=='\n'){
            j = 0;
            start = 0;
            en = 1;
            USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
            
        }
    }
	old_buf = RxBuffer;
}
void USART3_IRQHandler(){
    RxBuffer = (uint8_t)USART_ReceiveData(USART3);
    array[i] = RxBuffer;
    i++;
    if(i==7) i = 0;
}
void ADC_Configuration(){
	ADC_InitTypeDef			 			ADC_InitStructure;
	ADC_CommonInitTypeDef 		        ADC_CommonInitStructure;
	GPIO_InitTypeDef 					GPIO_InitStructure;
	
	/*enable clock*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	
	// PB0: ADC1-CH8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; /*Bo chia clock*/

    ADC_CommonInit(&ADC_CommonInitStructure);
    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b; // xem lai
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel configuration **************************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_4Cycles);
    /* Enable ADC */
    ADC_Cmd(ADC1, ENABLE);
    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartConv(ADC1);
}
double avergearray(int* arr, int number){
  int i=0, j=0;
  int max, min;
  double avg;
  long amount = 0;
	// sampling 
	while(j < number){
//		arr[j] = updateEstimate(ADC_GetConversionValue(ADC1));
        arr[j] = ADC_GetConversionValue(ADC1);
		j++;
	}
  if(number <= 0) return 0;
  if(number < 5){   //less than 5, calculated directly statistics
    for(i = 0;i < number;i++){
      amount += arr[i];
    }
    avg = amount/number;
    return avg;
  }
  else{
    if(arr[0]<arr[1]){
      min = arr[0];
			max=arr[1];
    }
    else{
      min=arr[1];
	  max=arr[0];
    }
    for(i = 2;i < number;i++){
      if(arr[i] < min){
        amount += min;        //arr < min
        min = arr[i];
      }
      else{
        if(arr[i] > max){
          amount += max;    //arr > max
          max = arr[i];
        }
        else{
          amount += arr[i]; // min <= arr <= max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
