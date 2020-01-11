/*
    File Name: 
    Author: khanh pham
    Date: 03-08-2019
*/
#include "stm32_rtc.h"

void RTC_Domain_access(void){
    
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
    PWR_RTCAccessCmd(ENABLE);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(ENABLE);
}

void RTC_setup(void){
    
    RTC_InitTypeDef rtc;
    
    rtc.RTC_HourFormat = RTC_HourFormat_24;
    rtc.RTC_AsynchPrediv = 0x7F;
    rtc.RTC_SynchPrediv = 0x120;
    RTC_WaitForSynchro();
    
    RTC_Init(&rtc);

}
void RTC_setupTime(uint8_t hour,uint8_t minute,uint8_t second){
   RTC_TimeTypeDef rtc_time;
    
   rtc_time.RTC_Hours = hour;
   rtc_time.RTC_Minutes = minute;
   rtc_time.RTC_Seconds = second;
    
   RTC_SetTime(RTC_Format_BIN,&rtc_time);

}
void RTC_setupDate(uint8_t weekday,uint8_t date,uint8_t month,uint8_t year){
   RTC_DateTypeDef rtc_date;
    
   rtc_date.RTC_Date = date;
   rtc_date.RTC_WeekDay = weekday;
   rtc_date.RTC_Month = month;
   rtc_date.RTC_Year = year;
   
   RTC_SetDate(RTC_Format_BIN,&rtc_date);
}

void RTC_Alarm_SetIT(){
   
    EXTI_InitTypeDef EXTI_alarm;
    NVIC_InitTypeDef NVIC_alarm;
    
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_alarm.EXTI_Line = EXTI_Line17;
    EXTI_alarm.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_alarm.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_alarm.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_alarm);
    
    /* Enable the RTC Alarm Interrupt */
    NVIC_alarm.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_alarm.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_alarm.NVIC_IRQChannelSubPriority = 0;
    NVIC_alarm.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_alarm);
    
   /*Enable the IT alarm A*/
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}

void RTC_Alarm_setTime(uint8_t m,uint8_t s){
    RTC_AlarmTypeDef alarmA; 
    /*config alarm*/
    RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
    alarmA.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours;
    alarmA.RTC_AlarmTime.RTC_Minutes = m;
    alarmA.RTC_AlarmTime.RTC_Seconds = s;
    RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&alarmA);
    /* Enable the alarmA */
    RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
}
