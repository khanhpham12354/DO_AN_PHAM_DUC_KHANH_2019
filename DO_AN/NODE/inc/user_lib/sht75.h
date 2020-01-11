#ifndef __SHT75_H
#define __SHT75_H

/******************************************************************************
 * Includes																																		*
 ******************************************************************************/
 
#include "stdint.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "delay.h"
#include "math.h"
#include <stddef.h>

#define CRC_ENA

// Enable ('1') or disable ('0') internal pullup on DATA line
// Commenting out this #define saves code space but leaves internal pullup
//   state undefined (ie, depends on last bit transmitted)
#define DATA_PU 1



/******************************************************************************
 * Definitions Macro  																												*
 ******************************************************************************/
 
#define _pinData 			GPIO_Pin_6
#define _pinClock			GPIO_Pin_5
#define GPIO_PORT           GPIOA
#define HIGH					1
#define LOW 					0
#define OUTPUT				    1
#define INPUT					0
#define NULL					0

/*macro function*/
#define pinMode(_pin, mode)	if(mode==1) GPIO_Config_Output(_pin); \
														else GPIO_Config_Input(_pin); 
	
#define digitalWrite(_pin, value)	GPIO_WriteBit(GPIO_PORT, _pin, (value))

// Clock pulse timing macros
// Lengthening these may assist communication over long wires
#define PULSE_LONG  delay_ms(3)
#define PULSE_SHORT delay_ms(1)

// Useful macros
#define SHT75_measTemp(result)  meas(TEMP, result, BLOCK)
#define SHT75_measHumi(result)  meas(HUMI, result, BLOCK)

// User constants
extern uint8_t     TEMP     ;
extern uint8_t     HUMI     ;
extern uint8_t     BLOCK    ;
extern uint8_t     NONBLOCK ;

// Status register bit definitions
extern uint8_t LOW_RES  ;  // 12-bit Temp / 8-bit RH (vs. 14 / 12)
extern uint8_t NORELOAD ;  // No reload of calibrarion data
extern uint8_t HEAT_ON  ;  // Built-in heater on
extern uint8_t BATT_LOW ;  // VDD < 2.47V

// Function return code definitions
extern uint8_t S_Err_NoACK  ;  // ACK expected but not received
extern uint8_t S_Err_CRC    ;  // CRC failure
extern uint8_t S_Err_TO     ;  // Timeout
extern uint8_t S_Meas_Rdy   ;  // Measurement ready

/******************************************************************************
 * Definitions function																												*
 ******************************************************************************/
uint8_t meas(uint8_t cmd, uint16_t *result, uint8_t block);
uint8_t measure(float *temp, float *humi, float *dew);
uint8_t measRdy(void);
uint8_t getResult(uint16_t *result);


uint8_t writeSR(uint8_t value);
uint8_t readSR(uint8_t *result);
uint8_t reset(void);
uint8_t putByte(uint8_t value);
uint8_t getByte(uint8_t ack);
void startTransmission(void);
void resetConnection(void);

float SHT75_calcTemp(uint16_t rawData);
float SHT75_calcHumi(uint16_t rawData, float temp);
float SHT75_calcDewpoint(float humi, float temp);
void calcCRC(uint8_t value, uint8_t *crc);
uint8_t bitrev(uint8_t value);

void GPIO_Config_Output(uint16_t);
void GPIO_Config_Input(uint16_t);

void SHT75_Setup(void);



#endif
