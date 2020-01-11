#include "sht75.h"

GPIO_InitTypeDef GPIO_InitStructure;
uint8_t _stat_reg = 0x00;
uint16_t *_presult = NULL;

// User constants
uint8_t     TEMP     =     0;
uint8_t     HUMI     =     1;
uint8_t     BLOCK    =     1;
uint8_t     NONBLOCK =     0;

// Status register bit definitions
uint8_t LOW_RES  =  0x01;  // 12-bit Temp / 8-bit RH (vs. 14 / 12)
uint8_t NORELOAD =  0x02;  // No reload of calibrarion data
uint8_t HEAT_ON  =  0x04;  // Built-in heater on
uint8_t BATT_LOW =  0x40;  // VDD < 2.47V

// Function return code definitions
uint8_t S_Err_NoACK  = 1;  // ACK expected but not received
uint8_t S_Err_CRC    = 2;  // CRC failure
uint8_t S_Err_TO     = 3;  // Timeout
uint8_t S_Meas_Rdy   = 4;  // Measurement ready


/******************************************************************************
 * Definitions
 ******************************************************************************/

// Sensirion command definitions:      adr command r/w
const uint8_t MEAS_TEMP   = 0x03;   // 000  0001   1
const uint8_t MEAS_HUMI   = 0x05;   // 000  0010   1
const uint8_t STAT_REG_W  = 0x06;   // 000  0011   0
const uint8_t STAT_REG_R  = 0x07;   // 000  0011   1
const uint8_t SOFT_RESET  = 0x1e;   // 000  1111   0

// Status register writable bits
const uint8_t SR_MASK     = 0x07;

// getByte flags
const uint8_t noACK  = 0;
const uint8_t ACK    = 1;

// Temperature & humidity equation constants
const float D1  =  -40.1;         // for deg C @ 5V
const float D2h =   0.01;         // for deg C, 14-bit precision
const float D2l =   0.04;         // for deg C, 12-bit precision

const float C1  = -2.0468;        // for V4 sensors
const float C2h =  0.0367;        // for V4 sensors, 12-bit precision
const float C3h = -1.5955E-6;     // for V4 sensors, 12-bit precision
const float C2l =  0.5872;        // for V4 sensors, 8-bit precision
const float C3l = -4.0845E-4;     // for V4 sensors, 8-bit precision

const float T1  =  0.01;          // for V3 and V4 sensors
const float T2h =  0.00008;       // for V3 and V4 sensors, 12-bit precision
const float T2l =  0.00128;       // for V3 and V4 sensors, 8-bit precision

/******************************************************************************
 * User functions																															*
 ******************************************************************************/
 // Setup 
 
 /*
 PA4 nguon cho sht75
 
 */
void SHT75_Setup(void){
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);	   // Enable clock
	 
   // Initialize CLK signal direction
	 _stat_reg = 0x00;
   // Note: All functions exit with CLK low and DAT in input mode
   pinMode(_pinClock, OUTPUT);

   // Return sensor to default state
   resetConnection();                // Reset communication link with sensor
   putByte(SOFT_RESET);              // Send soft reset command
}	
// Configuration _Pin as output
void GPIO_Config_Output(uint16_t _Pin){
	
	GPIO_InitStructure.GPIO_Pin = _Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIO_PORT, &GPIO_InitStructure);
}
// Configuration _Pin as input
void GPIO_Config_Input(uint16_t _Pin){
	
	GPIO_InitStructure.GPIO_Pin = _Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_Init(GPIO_PORT, &GPIO_InitStructure);
}


// All-in-one (blocking): Returns temperature, humidity, & dewpoint
uint8_t measure(float *temp, float *humi, float *dew) {
  uint16_t rawData;
  uint8_t error;
  if (error == SHT75_measTemp(&rawData))
    return error;
  *temp = SHT75_calcTemp(rawData);
  if (error == SHT75_measHumi(&rawData))
    return error;
  *humi = SHT75_calcHumi(rawData, *temp);
  *dew = SHT75_calcDewpoint(*humi, *temp);
  return 0 ;
}

// Initiate measurement.  If blocking, wait for result
#ifdef CRC_ENA
    uint8_t _crc;
#endif

uint8_t meas(uint8_t cmd, uint16_t *result, uint8_t block) {
  uint8_t error, i;
#ifdef CRC_ENA
  _crc = bitrev(_stat_reg & SR_MASK);  // Initialize CRC calculation
#endif
  startTransmission();
  if (cmd == TEMP)
    cmd = MEAS_TEMP;
  else
    cmd = MEAS_HUMI;
  if (error == putByte(cmd))
    return error;
#ifdef CRC_ENA
  calcCRC(cmd, &_crc);              // Include command byte in CRC calculation
#endif
  // If non-blocking, save pointer to result and return
  if (!block) {
    _presult = result;
    return 0;
  }
  // Otherwise, wait for measurement to complete with 720ms timeout
  i = 240;
  while (GPIO_ReadInputDataBit(GPIO_PORT, _pinData)) {
    i--;
    if (i == 0)
      return S_Err_TO;              // Error: Timeout
    delay_ms(3);
  }
  error = getResult(result);
  return error;
}

// Check if non-blocking measurement has completed
// Non-zero return indicates complete (with or without error)
uint8_t measRdy(void) {
  uint8_t error = 0;
  if (_presult == NULL)             // Already done?
    return S_Meas_Rdy;
  if (GPIO_ReadInputDataBit(GPIO_PORT, _pinData) != 0)   // Measurement ready yet?
    return 0;
  error = getResult(_presult);
  _presult = NULL;
  if (error)
    return error;                   // Only possible error is S_Err_CRC
  return S_Meas_Rdy;
}

// Get measurement result from sensor (plus CRC, if enabled)
uint8_t getResult(uint16_t *result) {
  uint8_t val;
#ifdef CRC_ENA
  val = getByte(ACK);
  calcCRC(val, &_crc);
  *result = val;
  val = getByte(ACK);
  calcCRC(val, &_crc);
  *result = (*result << 8) | val;
  val = getByte(noACK);
  val = bitrev(val);
  if (val != _crc) {
    *result = 0xFFFF;
    return S_Err_CRC;
  }
#else
  *result = getByte(ACK);
  *result = (*result << 8) | getByte(noACK);
#endif
  return 0;
}

// Write status register
uint8_t writeSR(uint8_t value) {
  uint8_t error;
  value &= SR_MASK;                 // Mask off unwritable bits
  _stat_reg = value;                // Save local copy
  startTransmission();
  if (error == putByte(STAT_REG_W))
    return error;
  return putByte(value);
}

// Read status register
uint8_t readSR(uint8_t *result) {
  uint8_t val;
  uint8_t error = 0;
#ifdef CRC_ENA
  _crc = bitrev(_stat_reg & SR_MASK);  // Initialize CRC calculation
#endif
  startTransmission();
  if (error == putByte(STAT_REG_R)) {
    *result = 0xFF;
    return error;
  }
#ifdef CRC_ENA
  calcCRC(STAT_REG_R, &_crc);       // Include command byte in CRC calculation
  *result = getByte(ACK);
  calcCRC(*result, &_crc);
  val = getByte(noACK);
  val = bitrev(val);
  if (val != _crc) {
    *result = 0xFF;
    error = S_Err_CRC;
  }
#else
  *result = getByte(noACK);
#endif
  return error;
}

// Public reset function
// Note: Soft reset returns sensor status register to default values
uint8_t reset(void) {
  _stat_reg = 0x00;                 // Sensor status register default state
  resetConnection();                // Reset communication link with sensor
  return putByte(SOFT_RESET);       // Send soft reset command & return status
}


/******************************************************************************
 * Sensirion data communication
 ******************************************************************************/

// Write byte to sensor and check for acknowledge
uint8_t putByte(uint8_t value) {
  uint8_t mask, i;
  uint8_t error = 0;
  pinMode(_pinData, OUTPUT);        // Set data line to output mode
  mask = 0x80;                      // Bit mask to transmit MSB first
  for (i = 8; i > 0; i--) {
    digitalWrite(_pinData, (value & mask));
    PULSE_SHORT;
    digitalWrite(_pinClock, HIGH);  // Generate clock pulse
    PULSE_LONG;
    digitalWrite(_pinClock, LOW);
    PULSE_SHORT;
    mask >>= 1;                     // Shift mask for next data bit
  }
  pinMode(_pinData, INPUT);         // Return data line to input mode
#ifdef DATA_PU
  digitalWrite(_pinData, DATA_PU);  // Restore internal pullup state
#endif
  digitalWrite(_pinClock, HIGH);    // Clock #9 for ACK
  PULSE_LONG;
  if (GPIO_ReadInputDataBit(GPIO_PORT, _pinData))        // Verify ACK ('0') received from sensor
    error = S_Err_NoACK;
  PULSE_SHORT;
  digitalWrite(_pinClock, LOW);     // Finish with clock in low state
  return error;
}

// Read byte from sensor and send acknowledge if "ack" is 1
uint8_t getByte(uint8_t ack) {
  uint8_t i;
  uint8_t result = 0;
  for (i = 8; i > 0; i--) {
    result <<= 1;                   // Shift received bits towards MSB
    digitalWrite(_pinClock, HIGH);  // Generate clock pulse
    PULSE_SHORT;
    result |= GPIO_ReadInputDataBit(GPIO_PORT, _pinData);  // Merge next bit into LSB position
		
    digitalWrite(_pinClock, LOW);
    PULSE_SHORT;
  }
  pinMode(_pinData, OUTPUT);
  digitalWrite(_pinData, !ack);     // Assert ACK ('0') if ack == 1
  PULSE_SHORT;
  digitalWrite(_pinClock, HIGH);    // Clock #9 for ACK / noACK
  PULSE_LONG;
  digitalWrite(_pinClock, LOW);     // Finish with clock in low state
  PULSE_SHORT;
  pinMode(_pinData, INPUT);         // Return data line to input mode
#ifdef DATA_PU
  digitalWrite(_pinData, DATA_PU);  // Restore internal pullup state
#endif
  return result;
}


/******************************************************************************
 * Sensirion signaling
 ******************************************************************************/

// Generate Sensirion-specific transmission start sequence
// This is where Sensirion does not conform to the I2C standard and is
// the main reason why the AVR TWI hardware support can not be used.
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
void startTransmission(void) {
	pinMode(_pinData, OUTPUT);     // output driver (avoid possible low pulse)
  digitalWrite(_pinData, HIGH);  // Set data register high before turning on
  PULSE_SHORT;
  digitalWrite(_pinClock, HIGH);
  PULSE_SHORT;
  digitalWrite(_pinData, LOW);
  PULSE_SHORT;
  digitalWrite(_pinClock, LOW);
  PULSE_LONG;
  digitalWrite(_pinClock, HIGH);
  PULSE_SHORT;
  digitalWrite(_pinData, HIGH);
  PULSE_SHORT;
  digitalWrite(_pinClock, LOW);
  PULSE_SHORT;
  // Unnecessary here since putByte always follows startTransmission
  pinMode(_pinData, INPUT);
}

// Communication link reset
// At least 9 SCK cycles with DATA=1, followed by transmission start sequence
//      ______________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
void resetConnection(void) {
  uint8_t i;
  pinMode(_pinData, OUTPUT);     // output driver (avoid possible low pulse)
  digitalWrite(_pinData, HIGH);  // Set data register high before turning on
  PULSE_LONG;
  for (i = 0; i < 9; i++) {
    digitalWrite(_pinClock, HIGH);
    PULSE_LONG;
    digitalWrite(_pinClock, LOW);
    PULSE_LONG;
  }
  startTransmission();
}


/******************************************************************************
 * Helper Functions
 ******************************************************************************/

// Calculates temperature in degrees C from raw sensor data
float SHT75_calcTemp(uint16_t rawData) {
  if (_stat_reg & LOW_RES)
    return D1 + D2l * (float) rawData;
  else
    return D1 + D2h * (float) rawData;
}

// Calculates relative humidity from raw sensor data
//   (with temperature compensation)
float SHT75_calcHumi(uint16_t rawData, float temp) {
  float humi;
  if (_stat_reg & LOW_RES) {
    humi = C1 + C2l * rawData + C3l * rawData * rawData;
    humi = (temp - 25.0) * (T1 + T2l * rawData) + humi;
  } else {
    humi = C1 + C2h * rawData + C3h * rawData * rawData;
    humi = (temp - 25.0) * (T1 + T2h * rawData) + humi;
  }
  if (humi > 100.0) humi = 100.0;
  if (humi < 0.1) humi = 0.1;
  return humi;
}

// Calculates dew point in degrees C
float SHT75_calcDewpoint(float humi, float temp) {
  float k;
  k = log(humi/100) + (17.62 * temp) / (243.12 + temp);
  return 243.12 * k / (17.62 - k);
}

#ifdef CRC_ENA
// Calculate CRC for a single byte
void calcCRC(uint8_t value, uint8_t *crc) {
  const uint8_t POLY = 0x31;   // Polynomial: x**8 + x**5 + x**4 + 1
  uint8_t i;
  *crc ^= value;
  for (i = 8; i > 0; i--) {
    if (*crc & 0x80)
      *crc = (*crc << 1) ^ POLY;
    else
      *crc = (*crc << 1);
  }
}

// Bit-reverse a byte (for CRC calculations)
uint8_t bitrev(uint8_t value) {
  uint8_t i;
  uint8_t result = 0;
  for (i = 8; i > 0; i--) {
    result = (result << 1) | (value & 0x01);
    value >>= 1;
  }
  return result;
}


#endif
